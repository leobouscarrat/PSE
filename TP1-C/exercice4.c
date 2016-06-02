#include "pse.h"

int main ( int argc, char *argv[]) {
  /* requete DNS */
  int code;
  struct addrinfo *infos, *curseur, hints;
  struct sockaddr_in *adresse;

  if (argc != 2) {
    erreur("usage: %s domaine\n", argv[0]);
  }

  memset ( &hints, 0, sizeof (struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  code = getaddrinfo ( argv[1], NULL, &hints, &infos );
  if ( code != 0) {
    erreur("getaddrinfo: %s\n", gai_strerror(code));
  }
  curseur = infos;
  while (curseur != NULL) {
    char adrIP[INET_ADDRSTRLEN];
    adresse = (struct sockaddr_in *) curseur->ai_addr;

    inet_ntop ( AF_INET, (const void *) &adresse->sin_addr, adrIP,
                INET_ADDRSTRLEN );
    printf("adr %s (0x%X)\n", adrIP, ntohl(adresse->sin_addr.s_addr));

    curseur = curseur->ai_next;
  }

  freeaddrinfo ( infos );
  exit(EXIT_SUCCESS);
}
