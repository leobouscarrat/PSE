#include "pse.h"
#include "tampon.h"

#define NBPROD  3
#define NBCONS  5

#define MILLISECONDES 1000
#define ATTENTECONS     1000*MILLISECONDES
#define ATTENTEPROD     2000*MILLISECONDES

pthread_mutex_t mutex_libre = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_plein = PTHREAD_MUTEX_INITIALIZER;

sem_t sCons, sProd;

Tampon tampon;

void *producteur(void *arg) {
  long idp = (long) arg;
  int arret = FAUX, valeur, indice = -1;

  printf("prod%ld: demarrage\n", idp);

  while (arret == FAUX) {
    /* production d'une donnee */
    valeur = time(NULL) / idp;
    printf("prod%ld: case %d, valeur produite %d\n", idp, indice, valeur);

    if ( sem_wait(&sProd) != 0 ) {
      erreur_pthread_IO("sem_wait sProd");
    }

    if ( pthread_mutex_lock(&mutex_libre) != 0 ) {
      erreur_pthread_IO("pthread_mutex_lock libre");
    }

    indice = tampon_deposer(&tampon, valeur);

    if ( pthread_mutex_unlock(&mutex_libre) != 0) {
      erreur_pthread_IO("pthread_mutex_unlock libre");
    }

    if ( sem_post(&sCons) != 0) {
      erreur_pthread_IO("sem_post sCpns");
    }
  }

  pthread_exit(NULL);
}

void *consommateur(void *arg) {
  long idc = (long) arg;
  int arret = FAUX, valeur, indice;

  printf("cons%ld: demarrage\n", idc);

  while (arret == FAUX) {
    if ( sem_wait(&sCons) != 0) {
      erreur_pthread_IO("sem_wait sCons");
    }

    if ( pthread_mutex_lock(&mutex_plein) != 0) {
      erreur_pthread_IO("pthread_mutex_lock plein");
    }

    indice = tampon_prendre(&tampon, &valeur);

    if ( pthread_mutex_unlock(&mutex_plein) != 0 ) {
      erreur_pthread_IO("pthread_mutex_unlock plein");
    }

    if ( sem_post(&sProd) != 0) {
      erreur_pthread_IO("sem_post sProd");
    }
    
    /* consommation donnee */
    usleep(ATTENTECONS);
    printf("cons%ld: valeur consommee %d de case %d\n", idc, valeur, indice);
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t tprod[NBPROD], tcons[NBCONS];
  long i;

  tampon_init(&tampon);
  sem_init(&sProd, 0, P);
  sem_init(&sCons, 0, 0);

  for (i=0; i<NBCONS; i++) {
    if (pthread_create(&tcons[i], NULL, consommateur, (void *) i+1) != 0) {
      erreur_IO("pthread_create");
    }
  }

  for (i=0; i<NBPROD; i++) {
    if (pthread_create(&tprod[i], NULL, producteur, (void *) i+1) != 0) {
      erreur_IO("pthread_create");
    }
  }

  printf("main: attente fin des producteurs/consommateurs\n");

  for (i=0; i<NBPROD; i++) {
    if (pthread_join(tprod[i], NULL) != 0) {
      erreur_pthread_IO("pthread_join prod");
    }
  }

  for (i=0; i<NBCONS; i++) {
    if (pthread_join(tcons[i], NULL) != 0) {
      erreur_pthread_IO("pthread_join cons");
    }
  }

  fflush(stdout);
  
  pthread_exit(EXIT_SUCCESS);
}

