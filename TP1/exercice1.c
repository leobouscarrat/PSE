#include "pse.h"

extern char ** environ;

int main ( int argc, char *argv[] ) {
	int i;
  printf("argc = %d\n", argc);
  if ( argc == 1) {
    erreur("%s: mauvais argument\n", argv[0]);
  }
for(i=0;i<argc;i++){
	printf("argv[%d] = \"%s\"\n",i,argv[i]);
}
  exit(EXIT_SUCCESS);
}
