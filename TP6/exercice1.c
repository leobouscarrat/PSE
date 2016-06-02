#include "pse.h"

int main(void) {
  char ligne[LIGNE_MAX];
  int continuer = VRAI, tube1, tube2, nblus;

  tube1 = open("tube1", O_RDONLY);
  if (tube1 == -1) {
    erreur_IO("open tube1");
  }

  tube2 = open("tube2", O_RDONLY);
  if (tube2 == -1) {
    erreur_IO("open tube2");
  }

  fprintf(stderr, "Lecture\n");

  while (continuer == VRAI) {
  
    nblus = lireLigne(tube1, ligne);
    if (nblus <= 0) {
      erreur("lecture tube1\n");
    }

    if (strcmp(ligne, "fin") == 0) continuer = FAUX;

    printf("ligne lue: \"%s\"\n", ligne);

    nblus = lireLigne(tube2, ligne);
    if (nblus <= 0) {
      erreur("lecture tube2\n");
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

