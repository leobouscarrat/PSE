#include "pse.h"

extern char ** environ;

int main ( int argc, char *argv[] ) {
	int i;
  printf("argc = %d\n", argc);
  if ( argc == 1) {
    erreur("%s: mauvais argument\n", argv[0]);
  }
for(i=0;*(environ +i) != NULL;i++){
	printf("env : %s \n",*(environ +i));
}
  exit(EXIT_SUCCESS);
}
