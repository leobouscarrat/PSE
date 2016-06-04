#include "pse.h"

extern char ** environ;

int main ( int argc, char *argv[] ) {
  char **ptr = environ;
  int i = 0;
  while (*ptr != NULL) {
    char *var = *ptr;
    printf("Variable %d: ", i++);
    while (*var != '\0') {
      printf("%c", *var);
      var++;
    }
    printf("\n");
    ptr++;
  }
  exit(EXIT_SUCCESS);
}
