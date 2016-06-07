#include "pse.h"

#define CMD   "client"
#define STDIN 0

void menu(void);
int crypto(int, char*); // fonction qui crypte/decrypte des fichiers
void generateChallenge(unsigned char* ,int); // generation de 16 octets aléatoirement
void viderBuffer(void);

int main(int argc, char *argv[]) 
{
  	int sock, arret = FAUX, ret, nbecr, nblus, affichage = FAUX, maxfd, signal = FAUX, nbsel;
  	struct sockaddr_in *sa;
  	char texte[LIGNE_MAX], mes[LIGNE_MAX];
  	char motDePasse[33];
  	fd_set fds;
  	system("clear");

  																			/////////////////////////////////////////////////////////////////////////////////////
  	if (argc != 3) 
  	{
    	erreur("usage: %s machine port\n", argv[0]);
  	}

  	printf("%s: creating a socket\n", CMD);
  	sock = socket (AF_INET, SOCK_STREAM, 0);
  	if (sock < 0) 
  	{
    	erreur_IO("socket");
  	}

  	printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
  	sa = resolv(argv[1], argv[2]);

  	if (sa == NULL) 
  	{															              //creation du socket de communication côté client

    	erreur("adresse %s et port %s inconnus\n", argv[1], argv[2]);
  	}
  	printf("%s: adr %s, port %hu\n", CMD,
	 	stringIP(ntohl(sa->sin_addr.s_addr)),
	 	ntohs(sa->sin_port));

  	/* connexion sur site distant */
  	printf("%s: connecting the socket\n", CMD);
  	ret = connect(sock, (struct sockaddr *) sa, sizeof(struct sockaddr_in));
  	if (ret < 0) 
  	{
   		erreur_IO("Connect");
  	}

  	freeResolv();															/////////////////////////////////////////////////////////////////////////////////////


  	/*Choix du nom d'utilisateur*/
	printf("Entrez votre nom d'utilisateur> ");

	if (fgets(texte, LIGNE_MAX, stdin) == NULL) 
	{
	  	printf("Fin de fichier (ou erreur) : arret.\n");
	  	arret = VRAI;
	}

	else 
	{
	 	nbecr = ecrireLigne(sock, texte);
	  	if (nbecr == -1) 
	  	{
			erreur_IO("ecrireLigne");										//enregistrement et envoie du pseudo du client
			arret = VRAI;
	 	}
		printf("Nom enregistré par le serveur\n");
		nblus = lireLigne(sock, texte);
		if (nblus == -1) 
		{
            erreur_IO("lireLigne");
        }
        else if (nblus == LIGNE_MAX) 
        {
            erreur("ligne trop longue\n");
        }
        else 
        {
        	printf("%s\n", texte);
        }
   	}

   	maxfd = sock;
       																		/////////////////////////////////////////////////////////////////////////////////////
  	while (arret == FAUX) 
  	{
  		menu();
	    printf("ligne> ");
	    fflush(stdout);
	    signal = FAUX;
	    while(signal == FAUX)
	    {
		    FD_ZERO(&fds);
	        FD_SET(sock, &fds); 
	        FD_SET(STDIN, &fds);
	        nbsel = select(maxfd+1, &fds, NULL, NULL, NULL); 
	        if (nbsel == -1) 
	        {
			    perror("select"); // error occurred in select()
			} 
			else if (nbsel == 0) 
			{
			    continue;
			} 
			else 
			{
		        if (FD_ISSET(sock, &fds))
		        {
			      	viderBuffer();
		            nblus = lireLigne(sock, texte);
					if (nblus == -1) 
					{
			            erreur_IO("lireLigne");
			        }
			        else if (nblus == LIGNE_MAX) 
			        {
			            erreur("ligne trop longue\n");
			        }
			        else if (nblus == 0){
			        }
			        else
			        {
			        	printf("%s\n", texte);
			        	if (fgets(texte, LIGNE_MAX, stdin) == NULL) 
					    {
					      	printf("Fin de fichier (ou erreur) : arrêt.\n");
					      	arret = VRAI;
					      	continue;
					    }
					    else
					    {
					    	nbecr = ecrireLigne(sock, texte);
						    if (nbecr == -1) {
								erreur_IO("ecrireLigne");
					      	}
					      	if(strcmp(texte,"Y\n")==0)
					      	{
					      		affichage = FAUX;
					      		while(affichage == FAUX)
					      		{
						      		nblus = lireLigne(sock, texte);
									if (nblus == -1) 
									{
							            erreur_IO("lireLigne");
							        }
							        else if (nblus == LIGNE_MAX) 
							        {
							            erreur("ligne trop longue\n");
							        }
							        else if (nblus == 0)
							        {
							        }
							        else
							        {
							        	affichage = VRAI;
						      		}
						      	}
						      	strcpy(motDePasse, texte);
						      	printf("\nAffichage du message reçu :\n%s\n", motDePasse);


						      	//Maintenant il faut recevoir le truc à décrypter

					      	}
					      	else
					      	{
					      		printf("Vous avez refusé la demande.\n");
					      		signal = VRAI;
					      	}
					    }
				    }
				    signal = VRAI;
		        }
			    
		        if (FD_ISSET(STDIN, &fds))
		        {
				    if (fgets(texte, LIGNE_MAX, stdin) == NULL) 
				    {
				      	printf("Fin de fichier (ou erreur) : arrêt.\n");
				      	arret = VRAI;
				      	continue;
				    }
				    else
				    {
				      	nbecr = ecrireLigne(sock, texte);
				      	if (nbecr == -1) 
				      	{
							erreur_IO("ecrireLigne");
				      	}
				      	if (strcmp(texte, "/fin\n") == 0) 
				      	{
							printf("Client. arret demande.\n");
							arret = VRAI;
				      	}
				      	else if (strcmp(texte, "1\n") == 0)
				      	{
				      		affichage = FAUX;
				      		system("clear");
				      		printf("\nAffichage liste des utilisateurs connectés :\n");
				      		while(affichage == FAUX)
				      		{
					      		nblus = lireLigne(sock, texte);
						      	if (nblus == -1) 
						      	{
					                erreur_IO("lireLigne");
					            }
					            else if (nblus == LIGNE_MAX) 
					            {
					                erreur("ligne trop longue\n");
					            }
					            else 
					            {
					            	if(strcmp(texte, "FIN")==0)
					            	{
					            		affichage = VRAI;
					            	}
					            	else 
					            	{
						            	printf("%s\n", texte);
						      		}
					      		}
				            }
				            printf("Appuyez sur la touche entrée pour revenir au menu\n");
				            getchar();
				      	}
				      	else if (strcmp(texte, "2\n") == 0)
				      	{
				      		printf("\nA quel utilisateur voulez-vous envoyer votre message ? (mettre l'id)\n");
				      		if (fgets(texte, LIGNE_MAX, stdin) == NULL) 
				      		{
						      	printf("Fin de fichier (ou erreur) : arret.\n");
						      	arret = VRAI;
						      	continue;
						    }
						    else 
						    {                                                           //////////////////////////////////////////////////////////
					      		nbecr = ecrireLigne(sock, texte);
						      	if (nbecr == -1) 
						      	{
									erreur_IO("ecrireLigne");
						      	}
						      	affichage = FAUX;
						      	while (affichage == FAUX)
						      	{                                  										//On vérifie que l'id soit valide.
						      		nblus = lireLigne(sock, mes);
							      	if (nblus == -1) {
						                erreur_IO("lireLigne");
						            }
						            else if (nblus == LIGNE_MAX) 
						            {
						                erreur("ligne trop longue\n");
						            }
						            else if(nblus == 0);
						            else 
						            {
						            	affichage = VRAI;
						            }
						      	}                                                          //////////////////////////////////////////////////////////////////  
						      	if(strcmp(mes, "OK")==0)
						      	{ 
						      		printf("L'id est valide. Demande en cours.\n");        //////////////////////////////////////////////////////
					      			affichage = FAUX;                                      // On rentre ici si l'id est valide
					      			while (affichage == FAUX)
					      			{
							      		nblus = lireLigne(sock, mes);
								      	if (nblus == -1) 
								      	{
							                erreur_IO("lireLigne");
							            }
							            else if (nblus == LIGNE_MAX) 
							            {
							                erreur("ligne trop longue\n");
							            }
							            else if(nblus == 0);
							            else 
							            {
							            	affichage = VRAI;
							            }
							      	}
							      	if(strcmp(mes, "OK")==0)
							      	{
							      		printf("L'autre utilisateur a bien accepté votre demande.\n\n");

								        printf("Entrez le message a envoyé :\n");
								        if (fgets(mes, LIGNE_MAX, stdin) != NULL) 
										{
											ecrireLigne(sock,mes);
								            printf("Message envoyé !!\n");
								            getchar();
								      	}
								      	
						            }
							        else 
							        {
							        	printf("%s\n", mes);
							        }
						    	}
							    else 
							    {
							    	printf("L'id %s n'est pas valable.\n", texte);
							    }
					            printf("Appuyez sur la touche entrée pour revenir au menu\n");
					            getchar();
					        }
				      	}

				      	else if (strcmp(texte, "3\n") == 0)
				      	{
				      		printf("\nA quel utilisateur voulez-vous envoyer votre message ? (mettre l'id)\n");
				      		if (fgets(texte, LIGNE_MAX, stdin) == NULL) 
				      		{
						      	printf("Fin de fichier (ou erreur) : arret.\n");
						      	arret = VRAI;
						      	continue;
						    }
						    else 
						    {                                                           //////////////////////////////////////////////////////////
					      		nbecr = ecrireLigne(sock, texte);
						      	if (nbecr == -1) 
						      	{
									erreur_IO("ecrireLigne");
						      	}
						      	affichage = FAUX;
						      	while (affichage == FAUX){                                  //On vérifie que l'id soit valide.
						      		nblus = lireLigne(sock, mes);
							      	if (nblus == -1) {
						                erreur_IO("lireLigne");
						            }
						            else if (nblus == LIGNE_MAX) {
						                erreur("ligne trop longue\n");
						            }
						            else if(nblus == 0);
						            else {
						            	affichage = VRAI;
						            }
						      	}                                                          //////////////////////////////////////////////////////////////////  
						      	if(strcmp(mes, "OK")==0){ 
						      		printf("L'id est valide. Demande en cours.\n");        //////////////////////////////////////////////////////
					      			affichage = FAUX;                                      // On rentre ici si l'id est valide
					      			while (affichage == FAUX)
					      			{
							      		nblus = lireLigne(sock, mes);
								      	if (nblus == -1) 
								      	{
							                erreur_IO("lireLigne");
							            }
							            else if (nblus == LIGNE_MAX) 
							            {
							                erreur("ligne trop longue\n");
							            }
							            else if(nblus == 0);
							            else 
							            {
							            	affichage = VRAI;
							            }
							      	}
							      	if(strcmp(mes, "OK")==0)
							      	{
							      		printf("L'autre utilisateur a bien accepté votre demande.\n");
									    affichage = FAUX;
							      		printf("\nAffichage du mot de passe :\n");
							      		while(affichage == FAUX){
								      		nblus = lireLigne(sock, texte);
									      	if (nblus == -1) 
									      	{
								                erreur_IO("lireLigne");
								            }
								            else if (nblus == LIGNE_MAX) 
								            {
								                erreur("ligne trop longue\n");
								            }
								            else 
								            {
								            	if(strcmp(texte, "FIN")==0)
								            	{
								            		affichage = VRAI;
								            	}
								            	else 
								            	{
									            	printf("Vérifier que le fichier à crypter est bien dans le dossier de l'executable et posssède le nom : \"infile.txt\" \n");
									            	printf("Appuyez sur la touche entrée pour lancer le cryptage\n");
									            	getchar();
									            	sprintf(motDePasse,"%s",texte);
									            	crypto(0, motDePasse); 

									            	//La il faut envoyer vers le serveur.
									      		}
								      		}
								      	}
						            }
							        else 
							        {
							        	printf("%s\n", mes);
							        }
						    	}
							    else 
							    {
							    	printf("L'id %s n'est pas valable.\n", texte);
							    }
					            printf("Appuyez sur la touche entrée pour revenir au menu\n");
					            getchar();
					        }
				      	}
				      	else if (strcmp(texte, "4\n") == 0)
				      	{
				      		affichage = FAUX;
				      		printf("\nCryptage du fichier demandé\n");
				      		while(affichage == FAUX)
				      		{
					      		nblus = lireLigne(sock, texte);
						      	if (nblus == -1) 
						      	{
					                erreur_IO("lireLigne");
					            }
					            else if (nblus == LIGNE_MAX) 
					            {
					                erreur("ligne trop longue\n");
					            }
					            else 
					            {
					            	printf("Appuyez sur la touche entrée pour lancer le cryptage\n");
									getchar();
									sprintf(motDePasse,"%s",texte);
									crypto(0, motDePasse);
									affichage = VRAI;
					      		}
				            }
				            printf("Appuyez sur la touche entrée pour revenir au menu\n");
				            getchar();
				      	}
				      	else if (strcmp(texte, "5\n") == 0)
				      	{
				      		printf("\nDecryptage du fichier demandé\n");
				      	
					        printf("Appuyez sur la touche entrée pour lancer le cryptage\n");
							getchar();
							crypto(1, motDePasse);
				            
				            printf("Appuyez sur la touche entrée pour revenir au menu\n");
				            getchar();
				      	}
				     	else 
				     	{
							printf("Commande non reconnue\n");
				      	}
				      	signal = VRAI;
				    }
				}
		    }
	    }
  	}

  	exit(EXIT_SUCCESS);
}

void menu (void) // affichage du Menu
{
	printf("   ----------------------------------------\n");
	printf("  |                  Menu                  |\n");
	printf("  |                                        |\n");
	printf("  |                                        |\n");
	printf("  | 1  Afficher la liste des utilisateurs  |\n");
	printf("  | 2  Envoyer un message à un utilisateur |\n");
	printf("  | 3  Envoyer un fichier a un utilisateur |\n");
	printf("  | 4  Cryptage du fichier linux.png       |\n");
	printf("  | 5  Decryptage du fichier crypto.dat    |\n");
	printf("  |                                        |\n");
	printf("  | /fin  Se deconnecter                   |\n");
	printf("  |                                        |\n");
	printf("   ----------------------------------------\n");
	
}

//pour générer 16 octets aléatoirement
void generateChallenge(unsigned char *challenge,int chl_size)
{

    int r = 0;

    r = RAND_bytes(challenge,chl_size);

    if (!r) 
    {
        printf("\nInternal error !\n");
        exit(EXIT_FAILURE);
    }

}

//fonction pour chiffrer/déchiffrer
int crypto(int mode, char* password)
{
    char *key_data=password;
    /* Allow enough space in output buffer for additional block */
    unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH],
                  key[32], iv[32], salt[16];

    int key_data_len = strlen(key_data), nrounds = 14, inlen, outlen;
    if (mode!=1)
    {
        FILE *in = fopen("linux.png","rb");
        FILE *out = fopen("crypto.dat","wb");
        generateChallenge(salt,16);
        fwrite(salt, 1, 16, out);

        //derivate key & iv from the supplied password
        EVP_BytesToKey(EVP_aes_256_cbc(), EVP_md5(), salt, (unsigned char*)key_data, key_data_len, nrounds, key, iv);

        EVP_CIPHER_CTX ctx;
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv,1);

        for(;;)
        {
         inlen = fread(inbuf, 1, 1024, in);
            if(inlen <= 0) break;
            if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
            {
                /* Error */
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
            }
            fwrite(outbuf, 1, outlen, out);
        }

        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
        {
            /* Error */
            EVP_CIPHER_CTX_cleanup(&ctx);
            return 0;
        }

        fwrite(outbuf, 1, outlen, out);
        EVP_CIPHER_CTX_cleanup(&ctx);

        fclose(in);
        fclose(out); 
    }
    else
    {
        FILE *in2 = fopen("crypto.dat","rb");
        FILE *out2 = fopen("decrypto.png","wb");

        fread(salt, 1, 16, in2);

        //derivate key & iv from the supplied password
        EVP_BytesToKey(EVP_aes_256_cbc(), EVP_md5(), salt, (unsigned char*)key_data, key_data_len, nrounds, key, iv);

        EVP_CIPHER_CTX ctx2;
        EVP_CIPHER_CTX_init(&ctx2);
        EVP_CipherInit_ex(&ctx2, EVP_aes_256_cbc(), NULL, key, iv, 0);

        for(;;)
        {
            inlen = fread(inbuf, 1, 1024, in2);
            if(inlen <= 0) break;
            if(!EVP_CipherUpdate(&ctx2, outbuf, &outlen, inbuf, inlen))
            {
                /* Error */
                EVP_CIPHER_CTX_cleanup(&ctx2);
                return 0;
            }
            fwrite(outbuf, 1, outlen, out2);
        }

        if(!EVP_CipherFinal_ex(&ctx2, outbuf, &outlen))
        {
            /* Error */
            EVP_CIPHER_CTX_cleanup(&ctx2);
            return 0;
        }
        fwrite(outbuf, 1, outlen, out2);
        EVP_CIPHER_CTX_cleanup(&ctx2);

        fclose(in2);
        fclose(out2);
    }

    return 1;
}

void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}
