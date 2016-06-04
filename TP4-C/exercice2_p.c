#include "pse.h"

int main (void) {
  pid_t pid;
  int status;
  pid = fork ();
  if (pid == -1) {
    erreur_IO("fork");
  }
  else if (pid == 0) {
    extern char **environ;
    char *param[2] = { "exercice2_f", NULL };
    execve(param[0], param, environ);
    erreur_IO("execve");
  }
  else {
    printf ("je suis le père %d (fils %d)\n", getpid(), pid);
    wait (&status);
    if (WIFEXITED (status)) { /* terminaison normale */
      printf ("code du fils %d = %d\n", pid, WEXITSTATUS (status));
    }
  }
  exit(EXIT_SUCCESS);
}
