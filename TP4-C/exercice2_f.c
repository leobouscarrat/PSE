#include "pse.h"

int main (void) {
  printf ("je suis le fils %d (père %d)\n", getpid(), getppid());
  exit(111);
}
