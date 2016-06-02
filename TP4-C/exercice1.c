#include "pse.h"

int main (void) {
  pid_t pid;
  int status;
  pid = fork ();
  if (pid == -1) {
    erreur_IO("fork");
  }
  else if (pid == 0) {
    printf ("je suis le fils %d (père %d)\n", getpid(), getppid());
    exit(111);
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
