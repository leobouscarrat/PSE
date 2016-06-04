#include "pse.h"

void * maFonction (void * val) {
  char * chaine = (char *) val;
  printf ("chaine recue: %s\n", chaine);
  /* attention : ne pas retourner l'adresse d'une variable locale !!! */
  pthread_exit((void *) strlen(chaine));
}

int main(void) {
  pthread_t leThread;
  int pcode;

  int ret = pthread_create (&leThread, NULL, maFonction, "Bonjour a tous");
  if (ret != 0) {
    erreur_IO ("pthread_create");  
  }

  if (pthread_join(leThread, (void *) &pcode) != 0) {
    erreur_IO ("pthread_join");
  }

  printf("main: le thread a retourne %d\n", pcode);

  pthread_exit(NULL);
}
