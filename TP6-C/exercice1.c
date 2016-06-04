#include "pse.h"

int main(void) {
  char ligne[LIGNE_MAX];
  int continuer = VRAI, tube1, tube2, nblus, nbfds, lec, max;
  fd_set rdfds;

  tube1 = open("tube1", O_RDONLY);
  if (tube1 == -1) {
    erreur_IO("open tube1");
  }

  tube2 = open("tube2", O_RDONLY);
  if (tube2 == -1) {
    erreur_IO("open tube2");
  }

  max = (tube1 > tube2) ? tube1 : tube2;
  max++;

  fprintf(stderr, "Lecture\n");

  while (continuer == VRAI) {
    FD_ZERO(&rdfds);
    FD_SET(tube1, &rdfds);
    FD_SET(tube2, &rdfds);

    nbfds = select(max, &rdfds, NULL, NULL, NULL);
    if (nbfds == -1) {
      erreur_IO("select");
    }
  
    if (nbfds == 2) {
      printf("les deux tubes sont prets... comment avez-vous fait ?!\n");
      lec = tube1;
    }
    else {
      if (FD_ISSET(tube1, &rdfds)) {
	lec = tube1;
	printf("tube1 est pret\n");
      }
      else {
	lec = tube2;
	printf("tube2 est pret\n");
      }
    }

    nblus = lireLigne(lec, ligne);
    if (nblus <= 0) {
      erreur("lecture %d\n", lec);
    }

    if (strcmp(ligne, "fin") == 0) continuer = FAUX;

    printf("ligne lue: \"%s\"\n", ligne);
  }
  
  if (close(tube1) == -1) {
    erreur_IO("close tube1");
  }

  if (close(tube2) == -1) {
    erreur_IO("close tube2");
  }

  return EXIT_SUCCESS;
}

