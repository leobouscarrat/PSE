#include "pse.h"

int main ( int argc, char *argv[] ) {
  int i = 0;
  printf("argc = %d\n", argc);
  for (i=0; i<argc; i++) {
    printf("argv[%d] =  %s\n", i, argv[i]);
  }
  if ( argc == 1) {
    erreur("nombre argument (%d) errone\n", argc);
  }
  exit(EXIT_SUCCESS);
}
