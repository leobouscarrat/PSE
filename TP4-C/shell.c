#include "pse.h"
#include "commande.h"

int main() {
  int code, fils, status, continuer = VRAI;

  /* astuce pour initialiser a zero la structure */
  Commande com = {0};

  while (continuer) {
    /* recuperation des codes de retour des fils termines */
    while ((fils = waitpid (-1, &status, WNOHANG)) > 0) {
      if (WIFEXITED(status)) { 
	printf("\t==> code du fils %d = %d\n", fils, WEXITSTATUS(status));
      }
    }
    /* si erreur, un code different de ECHILD est vraiment une erreur */
    if ( fils == -1 && errno != ECHILD ) {
      erreur_IO ("wait");
    }
    
    /* affichage du prompt */
    printf("$> "); fflush(stdout);

    code = lire_et_traiter(&com);

    if (code <= 0 || strcmp(com.argv[0], "exit") == 0) {
      continuer = FAUX;
    }
    else {
      fils = fork();
      if (fils == -1) {
	erreur_IO("fork");
      }
      else if (fils == 0) { /* processus fils */
	execvp(com.argv[0], com.argv);
	erreur_IO("execve");
      }
      else { /* processus pere */
	printf("\t==> pere: lancement du fils %d\n", fils);
	if (com.deferred) {
	  printf("\t==> pere: execution differee du fils\n");
	}
	else {
          if (waitpid (fils, &status, 0) == -1) {
	    erreur_IO("wait");
	  }
	  if (WIFEXITED(status)) { /* terminaison normale */
	    printf("\t==> code du fils %d = %d\n", fils, WEXITSTATUS(status));
	  }
	}
      }
    }
  } /* fin while */

  if (code == -1) {
    erreur("erreur %d\n", code);
  }
  printf("Bye.\n");
  return EXIT_SUCCESS;
}

