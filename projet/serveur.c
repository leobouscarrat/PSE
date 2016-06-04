#include "pse.h"
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

int journal;
struct user *utilisateurs;

char *printTime(void); // La fonction permet de renvoyer une chaine de caractère du temps 
void ecrireLog(void);
void *traiterRequete(void *arg);
void ajouterPseudo(char *texte, int tid); // la fonction ajoute un Pseudo à la liste des utilisateurs

void *traiterRequete(void *arg) {
    DataSpec * data = (DataSpec *) arg;
    int arret = FAUX, nblus, mode, pseudo = FAUX, nbecr, i, reception = FAUX;
    char texte[LIGNE_MAX], mes[LIGNE_MAX],nom[LIGNE_MAX];
  
    mode = O_WRONLY | O_APPEND | O_CREAT | O_TRUNC;

    /*Dans un premier temps nous allons enregistrer le pseudo et voir
    si il n'existe pas déjà*/
    
    while (arret == FAUX) {
        while (pseudo == FAUX){
            nblus = lireLigne (data->canal, texte);
            if (nblus == -1) {
                erreur_IO("lireLigne");
            }
            else if (nblus == LIGNE_MAX) {
                erreur("ligne trop longue\n");
            }
            else {
                ajouterPseudo(texte, data->tid);
                printf("worker%d enregistré, l'id est %d et le pseudo est %s \n", data->tid, data->tid, texte);
                sprintf(mes, "Vous etes enregistre en tant que %s, votre id est %d\n", texte, data->tid);
                ecrireLog();
                if(ecrireLigne(journal,"Connexion d'un nouvel utilisateur : \n") == -1) {
	    			erreur_IO("ecrireLigne");
					}
                if (ecrireLigne(journal, texte) == -1) {
	    			erreur_IO("ecrireLigne");
					}
                nbecr = ecrireLigne(data->canal, mes);
                if (nbecr == -1) {
                    erreur_IO("ecrireLigne");
                    arret = VRAI;
                }
                pseudo = VRAI;
            }
        }
        nblus = lireLigne (data->canal, texte);
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
            if (strcmp(texte, "/fin") == 0) {
	           printf("worker%d: arret demande.\n", data->tid);
	           ecrireLog();
	           sprintf(nom,"L'utilisateur %s s'est déconnecté",utilisateurs[data->tid-1].pseudo);
<<<<<<< HEAD
	           if (ecrireLigne(journal, nom) == -1) {
=======
	           nblus = ecrireLigne(journal, nom);
               if (nblus == -1) {
>>>>>>> a1c275ae5c5a2cddc4cbeba9b35fbf2b3b77e6e9
	    			erreur_IO("ecrireLigne");
					}
	           arret = VRAI;
	           continue;
            }
            else if (strcmp(texte, "/init") == 0) {
	           printf("worker%d: remise a zero du journal demandee.\n", data->tid);
	           if (close(journal) == -1) {
	               erreur_IO("close journal");
	            }
	            journal = open("journal.log", mode, 0660);
	            if (journal == -1) {
	               erreur_IO("open trunc journal");
	            }
            }
            else if (strcmp(texte, "1") == 0){
                for(i = 0; i < NELEMS(utilisateurs); i++){
                    sprintf(mes, "%d.%s", utilisateurs[i].pid, utilisateurs[i].pseudo);
                    nbecr = ecrireLigne(data->canal, mes);
                    if (nbecr == -1) {
                        erreur_IO("ecrireLigne");
                        arret = VRAI;
                    }
                    reception = FAUX;
                    while(reception == FAUX){
                        nblus = lireLigne(data->canal, texte);
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
                            if(strcmp(texte, "OK")){
                                reception = VRAI;
                            }
                        }
                    }
                }
            }
            else {
            	ecrireLog();
	            nbecr = ecrireLigne(journal, texte);
                 if (nbecr == -1) {
                    erreur_IO("ecrireLigne");
                    arret = VRAI;
                }
	            printf("worker%d: ligne de %d octets ecrite dans le journal.\n", data->tid, nblus);
	            fflush(stdout);
            }
        }
    }
    if (close(data->canal) == -1) {
        erreur_IO("close");
    }
    data->libre = VRAI; /* indique au main que le thread a fini */
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    int ecoute, canal, ret, mode, numthread = 0;
    struct sockaddr_in adrEcoute, reception;
    socklen_t receptionlen = sizeof(reception);
    DataThread *data;
    short port;
    utilisateurs = malloc(0);

    if (argc != 2) {
        erreur("usage: %s port\n", argv[0]);
    }

    mode = O_WRONLY|O_APPEND|O_CREAT;
    journal = open("journal.log", mode, 0660);
    if (journal == -1) {
        erreur_IO("open journal");
    }

    port = (short) atoi(argv[1]);
  
    printf("server: creating a socket\n");
    ecoute = socket (AF_INET, SOCK_STREAM, 0);
    if (ecoute < 0) {
        erreur_IO("socket");
    }
  
    adrEcoute.sin_family = AF_INET;
    adrEcoute.sin_addr.s_addr = INADDR_ANY;
    adrEcoute.sin_port = htons(port);
    printf("server: binding to INADDR_ANY address on port %d\n", port);
    ret = bind (ecoute,  (struct sockaddr *) &adrEcoute, sizeof(adrEcoute));
    if (ret < 0) {
        erreur_IO("bind");
    }
  
    printf("server: listening to socket\n");
    ret = listen (ecoute, 20);
    if (ret < 0) {
        erreur_IO("listen");
    }

    while (VRAI) {
        printf("server: waiting to a connection\n");
        canal = accept(ecoute, (struct sockaddr *) &reception, &receptionlen);
        if (canal < 0) {
            erreur_IO("accept");
        }
        printf("server: adr %s, port %hu\n",
	        stringIP(ntohl(reception.sin_addr.s_addr)),
	        ntohs(reception.sin_port));

        data = ajouterDataThread();
        if (data == NULL) {
            erreur("allocation impossible\n");
        }   
    
        data->spec.tid = ++numthread;
        data->spec.canal = canal;
        ret = pthread_create(&data->spec.id, NULL, traiterRequete, &data->spec);
        if (ret != 0) {
            erreur_IO("pthread_create");
        }
        else { /* thread main */
            printf("server: worker %d cree\n", numthread);
      
            /* verification si des fils sont termines */
            ret = joinDataThread();
            if (ret > 0) printf("server: %d thread termine.\n", ret);
            fflush(stdout);
            continue;
        }
    }

    libererDataThread();

    exit(EXIT_SUCCESS);
}


void ajouterPseudo(char *texte, int tid){
    int l;
    l = NELEMS(utilisateurs);
    utilisateurs = realloc(utilisateurs, sizeof(utilisateurs[0])*l+1);
    strcpy(utilisateurs[l].pseudo, texte);
    utilisateurs[l].pid = tid;
}

char *printTime(void){

	time_t temps;
    struct tm date;

    time(&temps);
    date=*localtime(&temps);

	return asctime(&date);
}

void ecrireLog(void){

	char temps[LIGNE_MAX];

    sprintf(temps,"%s\n",printTime());

    if (ecrireLigne(journal, "--------------------------------------------------------------------------------\n") == -1) {
	    erreur_IO("ecrireLigne");
	}
    if (ecrireLigne(journal, temps) == -1) {
	    erreur_IO("ecrireLigne");
	}

}