#include "pse.h"

int main ( int argc, char *argv[]) {
  struct sockaddr_in *adresse;

  if (argc < 2 || argc > 3) {
    erreur("usage: %s domaine [service]\n", argv[0]);
  }

  if (argc == 2) {
    adresse = resolv (argv[1], NULL);
  }
  else {
    adresse = resolv(argv[1], argv[2]);
  }
  if ( adresse == NULL ) {
    erreur("%s: aucune adresse retournee pour %s\n", argv[0], argv[1]);
  }

  printf("adr %s (0x%X)", stringIP( ntohl(adresse->sin_addr.s_addr)),
	 ntohl(adresse->sin_addr.s_addr));
  if (argc == 3) {
    printf("\nport %hu (0x%hX)", ntohs(adresse->sin_port),
	   ntohs(adresse->sin_port));
  }
  printf("\n");
  freeResolv();
  exit(EXIT_SUCCESS);
}
