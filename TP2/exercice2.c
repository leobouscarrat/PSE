#include "pse.h"

int main(int argc, char *argv[]) {
  int inputFd, outputFd, openFlags, taille, numfich;
  mode_t filePerms;
  ssize_t numRead;
  char *buf, nomfich[5];
  
  if (argc != 4) {
    erreur("usage: %s fichier lettre taille\n", argv[0]);
  }

  numfich = 0;
  nomfich[0] = *argv[2];
  taille = atoi(argv[3]);
  buf = malloc(taille);
  if (buf == NULL) {
    erreur("allocation de %d octets impossible\n", taille);
  }

  /* ouverture du fichier source en mode lecture seule */
  inputFd = open(argv[1], O_RDONLY);
  if (inputFd == -1) {
    erreur_IO("open input");
  }

  openFlags = O_CREAT | O_WRONLY | O_TRUNC;
  /* les droits seront rw-r--r- */
  filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  /* le fichier sera ouvert avec le mode et les droits ci-dessus */

  while ( (numRead = read(inputFd, buf, taille)) > 0) {

    numfich++;
    sprintf(nomfich+1, "%3.3d", numfich);

    outputFd = open(nomfich, openFlags, filePerms);
    if (outputFd == -1) {
      erreur_IO("open output");
    }

    if (write(outputFd, buf, numRead) != numRead) {
      erreur_IO("write");
    }

    if (close(outputFd) == -1) {
      erreur_IO("close output");
    }
  }
  
  if (numRead == -1) {
    erreur_IO("read");
  }
  
  /* fermeture du fichier */
  if (close(inputFd) == -1) {
    erreur_IO("close input");
  }

  free(buf);

  exit(EXIT_SUCCESS);
}
