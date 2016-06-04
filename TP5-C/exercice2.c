#include "pse.h"

void * maFonction (void * val) {
  char * chaine = (char *) val;
  printf ("chaine recue: %s\n", chaine);
  pthread_exit(NULL);
}

int main(void) {
  pthread_t leThread;
  char *pcode;

  int ret = pthread_create (&leThread, NULL, maFonction, "Bonjour a tous");
  if (ret != 0) {
    erreur_IO ("pthread_create");  
  }

  if (pthread_join(leThread, (void *) &pcode) != 0) {
    erreur_IO ("pthread_join");
  }

  printf("main: le thread a retourne %p\n", pcode);

  exit(EXIT_SUCCESS);
}
