#include "pse.h"

#define NBTHREADS 10

pthread_t ident[NBTHREADS];
long sommes[NBTHREADS];

void * maFonction (void * val) {
  long i, id = (long) val, nombre = id + 1;
  sommes[id] = 0;
  for (i=1; i<=nombre; i++) {
    sommes[id] += i;
  }
  pthread_exit((void *) sommes[id]);
}

int main(void) {
  long i, pcode;

  for (i=0; i<NBTHREADS; i++) {
    int ret = pthread_create (&ident[i], NULL, maFonction, (void *) i);
    if (ret != 0) {
      erreur_IO ("pthread_create");  
    }
  }

  for (i=0; i<NBTHREADS; i++) {
    if (pthread_join(ident[i], (void *) &pcode) != 0) {
      erreur_IO ("pthread_join");
    }
    printf("main: le thread %ld a retourne %ld\n", i+1, pcode);
  }
  pthread_exit(NULL);
}
