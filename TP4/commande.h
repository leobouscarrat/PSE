#ifndef COMMANDE_INCLUDE_H
#define COMMANDE_INCLUDE_H

#define ARG_COUNT    10     /* nombre max d'arguments */

typedef struct Commande_ {
  int argc;                 /* nombre d'arguments */
  char *argv[ARG_COUNT];    /* les arguments */
  int deferred;             /* booleen, indique execution differee */
} Commande;

/*
 * Cette fonction lit une ligne au clavier et la decoupe en mots, separes
 * par le caractere ' ', qui sont stockes dans la structure com :
 *  - le champ argc contient le nombre de mots stockes dans argv
 *  - le champ argv est construit pour l'appel systeme exec
 *  - le champ deferred est à vrai si le caractere '&' est place en fin
 *    de ligne.
 *
 * En retour, la fonction retourne -2 si le nombre d'arguments est superieur
 * a ARG_COUNT, -1 en cas d'erreur de lecture (la fonction utilise lireLigne
 * du module ligne), 0 en cas de fin de fichier, sinon elle retourne 1.
 *
 * Contraintes d'interpretation :
 *  - ARG_COUNT arguments max (valeur 10)
 *  - seul ' ' est separateur de mots
 *  - pas d'interpretation de caracteres speciaux
 *  - pas de controle de syntaxe
 *  - les chaines de caractere de argv sont allouees par malloc, et sont
 *    liberees lors de l'utilisation suivante de la fonction...
 */
int lire_et_traiter(Commande *com);

#endif
