#include "pse.h"
#include <time.h>

#define MILLISECONDES 1000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/* variable contenant le tirage aleatoire (entre 1 et 6) */
int valeurAleatoire = 0;

void *produire(void *arg) {
  int i;
  char statearray[256];
  struct random_data rd;
  struct timespec attente = { 0, 200*MILLISECONDES };

  memset(&rd, 0, sizeof(rd));
  initstate_r(time(NULL), statearray, 256, &rd);

  for (i=0; i<3; i++) {
    if (pthread_mutex_lock(&mutex) != 0) {
      erreur_IO("pthread_mutex_lock");
    }
    printf("prod: section critique\n");
    
    nanosleep(&attente, NULL);
    random_r(&rd, &valeurAleatoire);
    valeurAleatoire = ((double) valeurAleatoire / RAND_MAX)*6+1+i*6;
    
    if (pthread_mutex_unlock(&mutex) != 0) {
      erreur_IO("pthread_mutex_unlock");
    }
  
    printf("prod: signal\n");

    if (pthread_cond_signal(&cond) != 0) {
      erreur("pthread_cond_signal\n");
    }
  }

  pthread_exit(NULL);
}

void *consommer(void *arg) {
   long id = (long) arg;
  struct timespec attente = { 0, 100*MILLISECONDES };
 
  if (pthread_mutex_lock(&mutex) != 0) {
    erreur_IO("pthread_mutex_lock");
  }
  printf("cons %ld: wait\n", id);
    
  while (valeurAleatoire == 0) {
    if (pthread_cond_wait(&cond, &mutex) != 0) {
      erreur_IO("pthread_cond_wait");
    }
  }
  
  printf("cons %ld: section critique\n", id);
    
  /* le mutex a ete verrouille par pthread_cond_wait */
    
  printf("cons %ld: nombre aleatoire %d\n", id, valeurAleatoire);
  nanosleep(&attente, NULL);

  if (pthread_mutex_unlock(&mutex) != 0) {
    erreur_IO("pthread_mutex_unlock");
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t tidprod, tidcons[3];
  long i;

  for (i=0; i<3; i++) {
    if (pthread_create(&tidcons[i], NULL, consommer, (void *) i) != 0) {
      erreur_IO("pthread_create");
    }
  }
  
  if (pthread_create(&tidprod, NULL, produire, NULL) != 0) {
    erreur_IO("pthread_create");
  }

  for (i=0; i<3; i++) {
    if (pthread_join(tidcons[i], NULL) != 0) {
      erreur_IO("pthread_join cons");
    }
  }

  if (pthread_join(tidprod, NULL) != 0) {
    erreur_IO("pthread_join prod");
  }

  fflush(stdout);
  pthread_exit(EXIT_SUCCESS);
}
