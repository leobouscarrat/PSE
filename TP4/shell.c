#include "pse.h"
#include "commande.h"

int main() {
  int code, fils, status, continuer = VRAI;

  /* astuce pour initialiser a zero la structure */
  Commande com = {0};

  while (continuer) {
    while ((fils = wait (&status)) != -1) {
      if (WIFEXITED(status)) { 
	printf("\code du fils %d = %d\n", fils, WEXITSTATUS(status));
      }
    }
    if ( errno != ECHILD ) {
      erreur_IO ("wait");
    }
   
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
	printf("\pere: lancement du fils %d\n", fils);
	if (com.deferred) {
	  printf("\pere: execution differee du fils\n");
	}
	else {
	  fils = wait(&status);
	  if (fils == -1) {
	    erreur_IO("wait");
	  }
	  if (WIFEXITED(status)) { 
	    printf("\code du fils %d = %d\n", fils, WEXITSTATUS(status));
	  }
	}
      }
    }
  } /* fin while */

  if (code == -1) {
    erreur("erreur %d\n", code);
  }
  printf("fin\n");
  return EXIT_SUCCESS;
}

