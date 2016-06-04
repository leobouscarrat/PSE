#ifndef TAMPON_INCLUDE_H
#define TAMPON_INCLUDE_H

#define P 20

/* Type Tampon a P cases */
typedef struct tampon_t  {
  int iLibre;       /* indice de la premiere case libre */
  int iPlein;       /* indice de la premiere case occupee */
  int buffer[P];    /* buffer contenant les valeurs produites et consommees */
} Tampon;

/* Initialisation du tampon t.
   Tous les champs sont initialises a 0 */
void tampon_init(Tampon *t);

/* Depot d'une valeur v dans le tampon t.
   Le champ iLibre est utilise pour stocker v.
   L'indice de la case ou est deposee la valeur est retourne. */
int tampon_deposer(Tampon *t, int v) ;

/* Recuperation d'une valeur v depuis le tampon t.
   Le champ iPlein est utilise pour recuperer v.
   L'indice de la case ou etait stockee la valeur v est retourne. */
int tampon_prendre(Tampon *t, int *v);

#endif

