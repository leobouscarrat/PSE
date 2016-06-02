#include "pse.h"

#define    CMD      "serveur"

int main(int argc, char *argv[]) {
  int ecoute, canal, ret, log, arret = FAUX, nblus, mode;
  char texte[LIGNE_MAX];
  struct sockaddr_in adrEcoute, reception;
  socklen_t receptionlen;
  short port;
  
  if (argc != 2) {
    erreur("usage: %s port\n", argv[0]);
  }

  mode = O_WRONLY|O_APPEND|O_CREAT;
  log = open("journal.log", mode, 0660);
  if (log == -1) {
    erreur_IO("open log");
  }

  port = (short) atoi(argv[1]);
  
  printf("%s: creating a socket\n", CMD);
  ecoute = socket (AF_INET, SOCK_STREAM, 0);
  if (ecoute < 0) {
    erreur_IO("socket");
  }
  
  adrEcoute.sin_family = AF_INET;
  adrEcoute.sin_addr.s_addr = INADDR_ANY;
  adrEcoute.sin_port = htons(port);
  printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  ret = bind (ecoute,  (struct sockaddr *) &adrEcoute, sizeof(adrEcoute));
  if (ret < 0) {
    erreur_IO("bind");
  }
  
  printf("%s: listening to socket\n", CMD);
  ret = listen (ecoute, 5);
  if (ret < 0) {
    erreur_IO("listen");
  }

  receptionlen = sizeof(reception);
  printf("%s: accepting a connection\n", CMD);
  canal = accept(ecoute, (struct sockaddr *) &reception, &receptionlen);
  if (canal < 0) {
    erreur_IO("accept");
  }
  printf("%s: adr %s, port %hu\n", CMD,
	 stringIP(ntohl(reception.sin_addr.s_addr)),
	 ntohs(reception.sin_port));

  while (arret == FAUX) {
    mode |= O_TRUNC;
    nblus = lireLigne (canal, texte);
    if (nblus == -1) {
      erreur_IO("lireLigne");
    }
    else if (nblus == LIGNE_MAX) {
      erreur("ligne trop longue\n");
    }
    else if (nblus == 0) {
      continue;
    }
    else {
      if (strcmp(texte, "fin") == 0) {
	printf("Serveur. arret demande.\n");
	arret = VRAI;
	continue;
      }
      else if (strcmp(texte, "init") == 0) {
	printf("Serveur. remise a zero du journal demandee.\n");
	if (close(log) == -1) {
	  erreur_IO("close log");
	}
	log = open("journal.log", mode, 0660);
	if (log == -1) {
	  erreur_IO("open trunc log");
	}
      }
      else {
	if (ecrireLigne(log, texte) == -1) {
	  erreur_IO("ecrireLigne");
	}
	printf("%s: ligne de %d octets ecrite dans le journal.\n", CMD, nblus);
      }
    }
  }

  exit(EXIT_SUCCESS);
}
