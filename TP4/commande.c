#include "pse.h"
#include "commande.h"

int lire_et_traiter(Commande *com) {
  char lignecom[LIGNE_MAX], *token, *temp;
  int code, i;
  
  /* (re)initialisation de la structure et liberation memoire eventuelle */
  com->argc = 0;
  for (i=0; i<ARG_COUNT; i++) {
    if (com->argv[i] != NULL) free(com->argv[i]);
    com->argv[i] = NULL;
  }
  com->deferred = FAUX;

  /* lecture ligne de commande au clavier */
  code = lireLigne(STDIN_FILENO, lignecom);
  if (code == LIGNE_EOF) return 0;
  else if (code == -1 || code == LIGNE_MAX) return -1;

  /* decoupage de la ligne en mots (token) separes par ' ' */
  token = strtok(lignecom, " ");
  while (token != NULL) {
    temp = malloc(strlen(token)+1);
    if (temp == NULL) {
      erreur_IO("malloc");
    }
    strcpy(temp, token);
    com->argv[com->argc] = temp;
    com->argc++;
    if (com->argc >= ARG_COUNT) {
      return -2;
    }
    token = strtok(NULL, " ");
  }

  if (com->argc == 0) {
    return 0;
  }

  /* test de presence de '&'. Si oui, on l'enleve de argv et on positionne
     le champ deferred a vrai */
  if (strcmp(com->argv[com->argc-1], "&") == 0) {
    free(com->argv[com->argc-1]);
    com->argv[com->argc-1] = NULL;
    com->argc--;
    com->deferred = VRAI;
  }
  return 1;
}

