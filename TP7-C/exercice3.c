#include "pse.h"
#include <time.h>

#define MILLISECONDES 1000
#define ATTENTEPROD 200*MILLISECONDES
#define ATTENTECONS 100*MILLISECONDES

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/* variable contenant le tirage aleatoire (entre 1 et 6) */
int valeurAleatoire = 0;

void *produire(void *arg) {
  char statearray[256];

  struct random_data rd;

  memset(&rd, 0, sizeof(rd));
  initstate_r(time(NULL), statearray, 256, &rd);

  if (pthread_mutex_lock(&mutex) != 0) {
    erreur_IO("pthread_mutex_lock");
  }
  printf("prod: section critique\n");
  
  usleep(ATTENTEPROD);
  random_r(&rd, &valeurAleatoire);
  valeurAleatoire = ((double) valeurAleatoire / RAND_MAX)*6+1;
    
  if (pthread_mutex_unlock(&mutex) != 0) {
    erreur_IO("pthread_mutex_unlock");
  }
  
  printf("prod: signal\n");

  if (pthread_cond_signal(&cond) != 0) {
    erreur("pthread_cond_signal\n");
  }

  pthread_exit(NULL);
}

void *consommer(void *arg) {
  
  if (pthread_mutex_lock(&mutex) != 0) {
    erreur_IO("pthread_mutex_lock");
  }
  printf("cons: wait\n");
    
  while (valeurAleatoire == 0) {
    if (pthread_cond_wait(&cond, &mutex) != 0) {
      erreur_IO("pthread_cond_wait");
    }
  }
  
  printf("cons: section critique\n");
    
  /* le mutex a ete verrouille par pthread_cond_wait */
    
  printf("cons: nombre aleatoire %d\n", valeurAleatoire);
  usleep(ATTENTECONS);

  if (pthread_mutex_unlock(&mutex) != 0) {
    erreur_IO("pthread_mutex_unlock");
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t tidprod, tidcons;

  if (pthread_create(&tidcons, NULL, consommer, NULL) != 0) {
    erreur_IO("pthread_create");
  }
  
  if (pthread_create(&tidprod, NULL, produire, NULL) != 0) {
    erreur_IO("pthread_create");
  }
  
  if (pthread_join(tidcons, NULL) != 0) {
    erreur_IO("pthread_join prod");
  }

  if (pthread_join(tidprod, NULL) != 0) {
    erreur_IO("pthread_join prod");
  }

  fflush(stdout);
  pthread_exit(EXIT_SUCCESS);
}
