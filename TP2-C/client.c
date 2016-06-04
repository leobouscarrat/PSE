#include "pse.h"

int main(int argc, char *argv[]) {
  int fifo, arret = FAUX, nbecr;
  char texte[LIGNE_MAX];
  
  signal(SIGPIPE, SIG_IGN);

  fifo = open("fifo", O_WRONLY);
  if (fifo == -1) {
    erreur_IO("open fifo");
  }
 
  while (arret == FAUX) {
    printf("ligne> ");
    if (fgets(texte, LIGNE_MAX, stdin) == NULL) {
      printf("Fin de fichier (ou erreur) : arret.\n");
      arret = VRAI;
      continue;
    }
    else {
      nbecr = ecrireLigne(fifo, texte);
      if (nbecr == -1) {
	erreur_IO("ecrireLigne");
      }
      if (strcmp(texte, "fin\n") == 0) {
	printf("Client. arret demande.\n");
	arret = VRAI;
      }
      else {
	printf("Client. ligne de %d octets envoyee au serveur.\n", nbecr);
      }
    }
  }

  exit(EXIT_SUCCESS);
}
