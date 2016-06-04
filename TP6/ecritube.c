#include "pse.h"

int main(int n, char *a[]) {
  int tube, continuer = VRAI;
  char ligne[LIGNE_MAX];

  if (n != 2) {
    erreur("usage: ecritube tube[1|2]\n");
  }

  tube = open(a[1], O_WRONLY);
  if (tube == -1) {
    erreur_IO("open tube");
  }

  while (continuer == VRAI) {
    printf("Texte à ecrire dans le tube : ");
    fflush(stdout);
    
    if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
      erreur("lecture");

    if (strcmp(ligne, "fin\n") == 0)
      continuer = FAUX;
    
    ecrireLigne(tube, ligne);
  }

  if (close(tube) == -1) {
    erreur_IO("close");
  }

  return EXIT_SUCCESS;
}

