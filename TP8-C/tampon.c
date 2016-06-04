#include "pse.h"
#include "tampon.h"

void tampon_init(Tampon *t) {
  int i;
  t->iLibre = 0;
  t->iPlein = 0;
  for (i=0; i<P; i++) t->buffer[i] = 0;
}

int tampon_deposer(Tampon *t, int valeur) {
  int indice;
  indice = t->iLibre;
  t->buffer[t->iLibre] = valeur;
  t->iLibre = (t->iLibre + 1) % P;
  return indice;
}

int tampon_prendre(Tampon *t, int *valeur) {
  int indice;
  indice = t->iPlein;
  *valeur = t->buffer[t->iPlein];
  t->iPlein = (t->iPlein + 1) % P;
  return indice;
}

