#include "pse.h"

#define CMD   "client"

void menu(void);

int main(int argc, char *argv[]) {
  	int sock, arret = FAUX, ret, nbecr, nblus;
  	struct sockaddr_in *sa;
  	char texte[LIGNE_MAX];
  
  	if (argc != 3) {
    	erreur("usage: %s machine port\n", argv[0]);
  	}

  	printf("%s: creating a socket\n", CMD);
  	sock = socket (AF_INET, SOCK_STREAM, 0);
  	if (sock < 0) {
    	erreur_IO("socket");
  	}

  	printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
  	sa = resolv(argv[1], argv[2]);
  	if (sa == NULL) {
    	erreur("adresse %s et port %s inconnus\n", argv[1], argv[2]);
  	}
  	printf("%s: adr %s, port %hu\n", CMD,
	 	stringIP(ntohl(sa->sin_addr.s_addr)),
	 	ntohs(sa->sin_port));

  	/* connexion sur site distant */
  	printf("%s: connecting the socket\n", CMD);
  	ret = connect(sock, (struct sockaddr *) sa, sizeof(struct sockaddr_in));
  	if (ret < 0) {
   		erreur_IO("Connect");
  	}

  	freeResolv();


  	/*Choix du nom d'utilisateur*/
	printf("Entrez votre nom d'utilisateur> ");

	if (fgets(texte, LIGNE_MAX, stdin) == NULL) {
	  	printf("Fin de fichier (ou erreur) : arret.\n");
	  	arret = VRAI;
	}
	else {
	 	nbecr = ecrireLigne(sock, texte);
	  	if (nbecr == -1) {
			erreur_IO("ecrireLigne");
			arret = VRAI;
	 	}
		printf("Nom enregistré par le serveur\n");
		nblus = lireLigne(sock, texte);
		if (nblus == -1) {
                erreur_IO("lireLigne");
            }
            else if (nblus == LIGNE_MAX) {
                erreur("ligne trop longue\n");
            }
            else {
            	printf("%s\n", texte);
            }
       	}

  	while (arret == FAUX) {
  		menu();
	    printf("ligne> ");
	    if (fgets(texte, LIGNE_MAX, stdin) == NULL) {
	      	printf("Fin de fichier (ou erreur) : arret.\n");
	      	arret = VRAI;
	      	continue;
	    }
	    else {
	      	nbecr = ecrireLigne(sock, texte);
	      	if (nbecr == -1) {
				erreur_IO("ecrireLigne");
	      	}
	      	if (strcmp(texte, "/fin\n") == 0) {
				printf("Client. arret demande.\n");
				arret = VRAI;
	      	}
	     	else {
				printf("%s: ligne de %d octets envoyee au serveur.\n", CMD, nbecr);
	      	}
	    }
  	}

  	exit(EXIT_SUCCESS);
}

void menu (void)
{
	printf(" ----------------------------------------\n");
	printf("|                  Menu                  |\n");
	printf("|                                        |\n");
	printf("|                                        |\n");
	printf("| 1. Afficher la liste des utilisateurs  |\n");
	printf("| 2. Envoyer un message à un utilisateur |\n");
	printf("| 3. Envoyer un fichier a un utilisateur |\n");
	printf("| Q. Se deconnecter                      |\n");
	printf("|                                        |\n");
	printf(" ----------------------------------------\n");
	
}