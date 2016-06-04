#include "pse.h"

int main(int argc, char *argv[]) {
  int fifo, log, arret = FAUX, nblus, mode;
  char texte[LIGNE_MAX];
  
  fifo = open("fifo", O_RDONLY);
  if (fifo == -1) {
    erreur_IO("open fifo");
  }
 
  mode = O_WRONLY|O_APPEND|O_CREAT;
  log = open("journal.log", mode, 0660);
  if (log == -1) {
    erreur_IO("open log");
  }

  while (arret == FAUX) {
    mode |= O_TRUNC;
    nblus = lireLigne (fifo, texte);
    if (nblus == -1) {
      erreur_IO("lireLigne");
    }
    else if (nblus == LIGNE_MAX) {
      erreur("ligne trop longue\n");
    }
    else if (nblus == 0) {
      continue;
    }
    else {
      if (strcmp(texte, "fin") == 0) {
	printf("Serveur. arret demande.\n");
	arret = VRAI;
	continue;
      }
      else if (strcmp(texte, "init") == 0) {
	printf("Serveur. remise a zero du journal demandee.\n");
	if (close(log) == -1) {
	  erreur_IO("close log");
	}
	log = open("journal.log", mode, 0660);
	if (log == -1) {
	  erreur_IO("open trunc log");
	}
      }
      else {
	if (ecrireLigne(log, texte) == -1) {
	  erreur_IO("ecrireLigne");
	}
	printf("Serveur. ligne de %d octets ecrite dans le journal.\n", nblus);
      }
    }
  }

  if (close(log) == -1) {
    erreur_IO("close log");
  }

  if (close(fifo) == -1) {
    erreur_IO("close fifo");
  }
  
  exit(EXIT_SUCCESS);
}
