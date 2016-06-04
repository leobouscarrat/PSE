#include "pse.h"
#include <time.h>

#define MILLISECONDES 1000000

sem_t vide ;
sem_t plein ;

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
    
    nanosleep(&attente, NULL);
    printf("prod: produire si valeur précédente déjà consommée \n");
    if (sem_wait(&vide) != 0) {
        erreur("sem_wait\n");
    }
    random_r(&rd, &valeurAleatoire);
    valeurAleatoire = ((double) valeurAleatoire / RAND_MAX)*6+1+i*6;
    nanosleep(&attente, NULL);
    
    printf("prod: signaler nouvelle valeur\n");
    if (sem_post(&plein) != 0) {
        erreur("sem_post\n");
    }
    

  }

  pthread_exit(NULL);
}

void *consommer(void *arg) {
   long id = (long) arg;
  struct timespec attente = { 0, 100*MILLISECONDES };
 
  printf("conso %ld : cherche à consommer \n", id);
  if (sem_wait(&plein) != 0) {
        erreur("sem_wait\n");
  }
  
  printf("cons %ld: une donnée disponible\n", id);
    
    
  printf("cons %ld: nombre aleatoire %d\n", id, valeurAleatoire);
  printf("cons %ld: on signale au producteur que la donnée a été consommée\n", id);

  nanosleep(&attente, NULL);
  if (sem_post(&vide) != 0) {
        erreur("sem_wait\n");
  }
  // Vaquer à ses occupations
  nanosleep(&attente, NULL);

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t tidprod, tidcons[3];
  long i;

  if (sem_init(&vide, 0, 1) != 0) {
      erreur("sem_init\n");
  }
  if (sem_init(&plein, 0, 0) != 0) {
      erreur("sem_init\n");
  }
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
