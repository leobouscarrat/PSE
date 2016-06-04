#include "pse.h"

int main ( int argc, char *argv[]) {
  /* requete DNS */
  int code;
  struct addrinfo *infos, hints;
  struct sockaddr_in *adresse;

  memset ( &hints, 0, sizeof (struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (argc == 3) {
    code = getaddrinfo ( argv[1], argv[2], &hints, &infos );
  }
  else {
    code = getaddrinfo ( "www.emse.fr", "http", &hints, &infos );
  }
  if ( code != 0) {
    erreur("getaddrinfo: %s\n", gai_strerror(code));
  }
  adresse = (struct sockaddr_in *) infos->ai_addr;

  printf("port %hd\n", ntohs(adresse->sin_port));
  printf("adr  0x%X\n", ntohl(adresse->sin_addr.s_addr));

  /* par defaut,
     adresse->sin_addr.s_addr soit contenir 0XC131ACC2),
                              ie 193.49.172.194 
     adresse->sin_port doit contenir 80 */

  freeaddrinfo ( infos );
  exit(EXIT_SUCCESS);
}
