/* programme (c) M Kerrisk,
   adapté par P. Lalevée */

#include "pse.h"

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
  int inputFd, outputFd, openFlags, totRead;
  mode_t filePerms;
  ssize_t numRead;
  char buf[BUF_SIZE];
  
  if (argc != 3) {
    erreur("usage: %s source destination\n", argv[0]);
  }
  
  /* ouverture du fichier source en mode lecture seule */
  inputFd = open(argv[1], O_RDONLY);
  if (inputFd == -1) {
    /* exemples d'erreurs possibles :
       - le fichier n'existe pas
       - pas le droit de le lire
       - nombre max de fichiers ouverts atteint */
    erreur_IO("open input");
  }

  /* le fichier sera ouvert en ecriture seule. S'il existe deja, il sera
     ecrase, sinon il sera cree */
  openFlags = O_CREAT | O_WRONLY | O_TRUNC;
  /* dans le cas de la creation, les droits seront rw-rw-rw */
  filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  /* le fichier sera ouvert avec le mode et les droits ci-dessus */
  outputFd = open(argv[2], openFlags, filePerms);
  if (outputFd == -1) {
    /* exemples d'erreurs possibles :
       - pas le droit d'ecrire
       - nombre max de fichiers ouverts atteints */
    erreur_IO("open output");
    exit(EXIT_FAILURE);
  }

  totRead = 0;
  /* tant que la lecture de BUF_SIZE octets sur le fichier 'inputFD'
     retourne un nombre positif (nombre d'octets lus superieur a zero,
     ce qui indique ni erreur, ni fin de fichier)... */
  while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
    totRead += numRead;
    /* ... ces numRead octets sont ecrits sur le fichier 'outputFD'... */
    if (write(outputFd, buf, numRead) != numRead) {
      /* ... si ce n'est pas le cas, cela signifie une erreur dans
	 le cas de fichier disque */
      erreur_IO("write");
    }
  }
  
  /* on sort de la boucle precedente soit en cas d'erreur (numRead vaut -1)
     soit en cas de fin de fichier (numRead vaut 0) ; comme ce dernier
     cas est normal, on gere l'erreur eventuelle */
  if (numRead == -1) {
    /* exemples d'erreurs possibles
       - erreur d'entree-sortie de bas niveau sur le peripherique :
         cle USB retiree brusquement, probleme materiel...
       - erreur memoire sur le buffer... */
    erreur_IO("read");
  }
  
  /* fermeture du fichier */
  if (close(inputFd) == -1) {
    /* erreurs possibles : une erreur d'E/S physique peut se produire
       lors de la liberation des buffers memoire */
    erreur_IO("close input");
  }

  /* fermeture du fichier */
  if (close(outputFd) == -1) {
    /* erreur possible : une erreur d'E/S physique peut se produire lors de
       l'ecriture des buffers memoire sur disque */
    erreur_IO("close output");
  }

  printf("Nombre d'octets lus et ecrits : %d\n", totRead);

  exit(EXIT_SUCCESS);
}
