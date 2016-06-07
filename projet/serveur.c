#include "pse.h"

int journal;
int taille=0; 
struct user *utilisateurs; // liste des utilisateurs
char motDePasse[33];   // mot de passe utilisé pour le cryptage

char *printTime(void); // La fonction permet de renvoyer une chaine de caractère du temps 
void ecrireLog(void); // entete pour update le log
void *traiterRequete(void *arg);
void ajouterPseudo(char *texte, int tid); // la fonction ajoute un Pseudo à la liste des utilisateurs
void deconnexion(int tid); //La fonction deconnexion met l'utilisateur en état deconnecte
void generateMdp(char*); // randomisateur de mot de passe 256 bits
int demandeEnvoi(int id_emetteur, int id_receveur);
int idValide(char * id);
void envoiMDP(char * texte, char * motDePasse);

pthread_cond_t condition = PTHREAD_COND_INITIALIZER; /* Création de la condition */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */

void *threadMdp (void* arg)  // thread qui génère des mots de passe aléatoires
{
    int i=0;
    while(1) /* Boucle infinie */
    {  
        pthread_mutex_lock (&mutex); /* On verrouille le mutex */
        for (i=0;i<10;i++)         //on génère 10 fois le mot de passe pendant que la variable est bloquée
            generateMdp(motDePasse);        
        pthread_mutex_unlock (&mutex); /* On déverrouille le mutex */
        pthread_cond_signal (&condition); /* On délivre le signal : condition remplie */
        usleep (10); /* On laisse 10 µsecondes de repos */
    }
    pthread_exit(NULL); /* Fin du thread */
}

void *traiterRequete(void *arg) 
{
    DataSpec * data = (DataSpec *) arg;
    int arret = FAUX, nblus, mode, pseudo = FAUX, nbecr, i, reception =FAUX;
    char texte[LIGNE_MAX], mes[LIGNE_MAX],nom[LIGNE_MAX];
  
    mode = O_WRONLY | O_APPEND | O_CREAT | O_TRUNC;

    /*Dans un premier temps nous allons enregistrer le pseudo*/
    
    while (arret == FAUX) 
    {
        while (pseudo == FAUX)
        {                                                                                         //////////////////////////////////////////////////////////////////////////
            nblus = lireLigne (data->canal, texte);                                         
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
                ajouterPseudo(texte, data->tid);                                                                        //ce bloc permet de récupérer le pseudo de l'utlisateur et de faire le log
                printf("worker%d enregistré, l'id est %d et le pseudo est %s \n", data->tid, data->tid, texte);         // de cette connexion
                sprintf(mes, "Vous etes enregistré en tant que %s, votre id est %d\n", texte, data->tid);
                ecrireLog();
                if(ecrireLigne(journal,"Connexion d'un nouvel utilisateur : \n") == -1) 
                {
	    			erreur_IO("ecrireLigne");
				}
                if (ecrireLigne(journal, texte) == -1) 
                {
	    			erreur_IO("ecrireLigne");
				}
                nbecr = ecrireLigne(data->canal, mes);
                if (nbecr == -1) 
                {
                    erreur_IO("ecrireLigne");
                    arret = VRAI;
                }
                pseudo = VRAI;
            }
        }                                                                                                  ////////////////////////////////////////////////////////////////////////////////

        if(utilisateurs[data->tid - 1].flag) 
        {
        strcpy(nom, utilisateurs[data->tid - 1].message);
        sprintf(texte, "L'utilisateur %s d'id %s veut vous envoyez un message. Acceptez-vous de le recevoir ? (Y/N)\n", utilisateurs[atoi(nom)-1].pseudo, nom);
        nbecr = ecrireLigne (data->canal, texte);
        if (nbecr == -1) 
        {
            erreur_IO("ecrireLigne");
            arret = VRAI;
        }
        reception=FAUX;
        while(reception==FAUX)
        {
            nblus = lireLigne(data->canal, texte);
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
                continue;
            }
            else 
            {
                reception = VRAI;
            }
        }
        if(strcmp(texte, "Y") == 0)
        {
            strcpy(utilisateurs[data->tid-1].message, "OK");
            utilisateurs[data->tid-1].flag = 2;
            while(utilisateurs[data->tid-1].flag != 1);
            strcpy(texte, utilisateurs[data->tid-1].message);
            nbecr = ecrireLigne(data->canal, texte);
            if (nbecr == -1) 
            {
                erreur_IO("ecrireLigne");
                arret = VRAI;
            }

                //Maintenant il faut recevoir les trucs à crypter depuis le serveur, et les renvoyer vers le client
        }
            else
            {
                strcpy(utilisateurs[data->tid-1].message, "NON");
                utilisateurs[data->tid-1].flag = 2;
            }
            utilisateurs[data->tid-1].flag = 0;
        }
        else 
        {
            nblus = lireLigne (data->canal, texte);
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
                continue;
            }
            else 
            {                                                                                                      /////////////////////////////////////////////////////////////////////////////////////
                if (strcmp(texte, "/fin") == 0) 
                {
    	           printf("worker%d: arret demandé.\n", data->tid);
    	           ecrireLog();
    	           sprintf(nom,"L'utilisateur %s s'est déconnecté",utilisateurs[data->tid-1].pseudo);
    	           nblus = ecrireLigne(journal, nom);
                   if (nblus == -1) 
                   {
    	    			erreur_IO("ecrireLigne");                                                                           // si le serveur recoit /fin de la part du worker X alors il ferme la connexion et
    		       }                                                                                                   // la thread du worker 
    	           arret = VRAI;
    	           continue;                                                                                              ////////////////////////////////////////////////////////////////////////////////////
                }
                else if (strcmp(texte, "/init") == 0) 
                {
    	           printf("worker%d: remise à zéro du journal demandée.\n", data->tid);
    	           if (close(journal) == -1) 
                   {
    	               erreur_IO("close journal");                                                                         // i le serveur recoit /fin de la part du worker X alors il réinitialise le log
    	           }
    	            journal = open("journal.log", mode, 0660);
    	            if (journal == -1) 
                    {
    	               erreur_IO("open trunc journal");
    	            }  
                }                                                                                                            ///////////////////////////////////////////////////////////////////////////////
                else if (strcmp(texte, "1") == 0)
                {
                    printf("worker%d: affichage de la liste des utilisateurs demandée.\n", data->tid);
                    ecrireLog();
                    sprintf(nom,"L'utilisateur %s a demandé l'affichage de la liste des users",utilisateurs[data->tid-1].pseudo);
                    nblus = ecrireLigne(journal, nom);
                    if (nblus == -1) 
                    {
                        erreur_IO("ecrireLigne");
                    }
                    
                    for(i = 0; i < taille; i++)
                    {
                        if(utilisateurs[i].connecte)
                        {
                            if(i == data->tid-1){sprintf(mes, "%d.%s : c'est moi !", i+1, utilisateurs[i].pseudo);}
                            else {sprintf(mes, "%d.%s", i+1, utilisateurs[i].pseudo);}
                            nbecr = ecrireLigne(data->canal, mes);
                            if (nbecr == -1) 
                            {
                                erreur_IO("ecrireLigne");                                                                       // si le serveur reçoit 1 il envoie la liste des utilisateurs au client
                                arret = VRAI;                                                                                   // comme FIN termine l'envoi, le client sait que la liste est complète
                            }
                        }
                    }                                           
                    nbecr = ecrireLigne(data->canal, "FIN\n");
                    if (nbecr == -1) 
                    {
                        erreur_IO("ecrireLigne");
                        arret = VRAI;
                    }                                                                                                           /////////////////////////////////////////////////////////////////////////////

                }
                else if (strcmp(texte, "2") == 0)
                {
                    reception = FAUX;
                    while(reception == FAUX){
                        nblus = lireLigne (data->canal, texte);
                        if (nblus == -1) 
                        {
                            erreur_IO("lireLigne");
                        }
                        else if (nblus == LIGNE_MAX) 
                        {
                            erreur("ligne trop longue\n");
                        }
                        else if (nblus == 0);
                        else 
                        {
                            reception = VRAI;
                        }
                    }
                    if(idValide(texte) == VRAI && atoi(texte)!=data->tid) 
                    {
                        nbecr = ecrireLigne(data->canal, "OK\n");
                        if (nbecr == -1) 
                        {
                            erreur_IO("ecrireLigne");
                        } 
                        if(demandeEnvoi(data->tid, atoi(texte))) 
                        {
                            nbecr = ecrireLigne(data->canal, "OK\n");
                            if (nbecr == -1) 
                            {
                                erreur_IO("ecrireLigne");
                            } 
                            nblus = lireLigne (data->canal, mes);
                            if (nblus == -1) 
                            {
                                erreur_IO("lireLigne");
                            }
                            else if (nblus == LIGNE_MAX) 
                            {
                                erreur("ligne trop longue\n");
                            }
                            else if (nblus == 0);
                            else 
                            {
                            envoiMDP(texte, mes);
                            }
                            do
                            {
                              envoiMDP(texte, mes);  
                            }
                            while(strcmp(mes,"ok")!=0); 

                            nbecr = ecrireLigne(data->canal, "FIN\n"); // FIN pour dire que l'envoi est terminé
                            if (nbecr == -1) 
                            {
                                erreur_IO("ecrireLigne");
                            }
                        }
                        else 
                        {
                            sprintf(mes, "L'utilisateur d'id %s a refusé de réceptionner votre fichier.", texte);
                            nbecr = ecrireLigne(data->canal, mes);
                            if (nbecr == -1) 
                            {
                                erreur_IO("ecrireLigne");
                            } 
                        }
                    }
                    else 
                    {
                        sprintf(mes, "L'id n'est pas valide ou pas disponible.");
                        nbecr = ecrireLigne(data->canal, mes);
                        if (nbecr == -1) 
                        {
                            erreur_IO("ecrireLigne");
                            arret = VRAI;
                        }   
                    }
                }
                else if (strcmp(texte, "3") == 0)
                {
                    reception = FAUX;
                    while(reception == FAUX)
                    {
                        nblus = lireLigne (data->canal, texte);
                        if (nblus == -1) 
                        {
                            erreur_IO("lireLigne");
                        }
                        else if (nblus == LIGNE_MAX) 
                        {
                            erreur("ligne trop longue\n");
                        }
                        else if (nblus == 0);
                        else 
                        {
                            reception = VRAI;
                        }
                    }
                    if(idValide(texte) == VRAI && atoi(texte)!=data->tid) 
                    {
                        nbecr = ecrireLigne(data->canal, "OK\n");
                        if (nbecr == -1) 
                        {
                            erreur_IO("ecrireLigne");
                        } 
                        if(demandeEnvoi(data->tid, atoi(texte))) 
                        {
                            nbecr = ecrireLigne(data->canal, "OK\n");
                            if (nbecr == -1) 
                            {
                                erreur_IO("ecrireLigne");
                            } 
                            printf("worker%d: génération de mot de passe.\n", data->tid);
                            ecrireLog();
                            sprintf(nom,"L'utilisateur %s a demandé un mot de passe aléatoire",utilisateurs[data->tid-1].pseudo);
                            nbecr = ecrireLigne(journal, nom);
                            if (nblus == -1) 
                            {
                                erreur_IO("ecrireLigne");
                            }  
                            pthread_mutex_lock(&mutex); /* On verrouille le mutex */
                            while(pthread_cond_wait (&condition, &mutex)); /* On attend que la condition soit remplie */
                            sprintf(mes, "%s", motDePasse);
                            pthread_mutex_unlock(&mutex); /* On déverrouille le mutex */
                            nbecr = ecrireLigne(data->canal, mes);
                            if (nbecr == -1) 
                            {
                                erreur_IO("ecrireLigne");
                                arret = VRAI;
                            }
                            envoiMDP(texte, motDePasse);
                        }     /////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        else 
                        {
                            sprintf(mes, "L'utilisateur d'id %s a refusé de réceptionner votre fichier.", texte);
                            nbecr = ecrireLigne(data->canal, mes);
                            if (nbecr == -1) 
                            {
                                erreur_IO("ecrireLigne");
                            } 
                        }
                    }
                    else 
                    {
                        sprintf(mes, "L'id n'est pas valide ou pas disponible.");
                        nbecr = ecrireLigne(data->canal, mes);
                        if (nbecr == -1) 
                        {
                            erreur_IO("ecrireLigne");
                            arret = VRAI;
                        }   
                    }
                } 
                else if (strcmp(texte, "4") == 0)
                {
                    printf("worker%d: génération de mot de passe.\n", data->tid);
                    ecrireLog();
                    sprintf(nom,"L'utilisateur %s a demandé un mot de passe aléatoire",utilisateurs[data->tid-1].pseudo);
                    nbecr = ecrireLigne(journal, nom);
                    if (nblus == -1) 
                    {
                        erreur_IO("ecrireLigne");
                    }
                    pthread_mutex_lock(&mutex); /* On verrouille le mutex */
                    while(pthread_cond_wait (&condition, &mutex)); /* On attend que la condition soit remplie */
                    sprintf(mes, "%s", motDePasse);
                    pthread_mutex_unlock(&mutex); /* On déverrouille le mutex */
                    nbecr = ecrireLigne(data->canal, mes);
                    if (nbecr == -1) 
                    {
                        erreur_IO("ecrireLigne");
                        arret = VRAI;
                    }
                }
                else 
                {
                	ecrireLog();
    	            nbecr = ecrireLigne(journal, texte);     // on log le texte envoyé s'il ne déclenche pas de commande
                    if (nbecr == -1) 
                    {
                        erreur_IO("ecrireLigne");
                        arret = VRAI;
                    }
    	            printf("worker%d: ligne de %d octets écrite dans le journal.\n", data->tid, nblus);
    	            fflush(stdout);                               /////////////////////////////////////////////////////////////////////////////
                }
            }
        }
    }
    if (close(data->canal) == -1) 
    {                         // fermeture du canal entre le client et le serveur
        erreur_IO("close");
    }
    deconnexion(data->tid);                                 // passage à l'état deconnecté pour le client 
    data->libre = VRAI; /* indique au main que le thread a fini */
    pthread_exit(NULL); // fermeture de la thread
}


int main(int argc, char *argv[]) 
{
    int ecoute, canal, ret, mode, numthread = 0;
    struct sockaddr_in adrEcoute, reception;
    socklen_t receptionlen = sizeof(reception);
    DataThread *data;
    short port;
    utilisateurs = malloc(0); // init de la liste des clients
    srand(time(NULL));  // initialisation du temps pour la génération aléatoire


    data = ajouterDataThread();                                                /////////////////////////////////////////////////////////////////////////////////////////////////
        if (data == NULL) 
        {
            erreur("allocation impossible\n");
        }   
    
        data->spec.tid = numthread;
        ret = pthread_create(&data->spec.id, NULL, threadMdp, &data->spec); 
        if (ret != 0) 
        {
            erreur_IO("pthread_create");                                            //creation de la thread de generation de mot de passe
        }
        else 
        { /* thread main */
            printf("server: worker %d créé : génération en cours\n", numthread);
      
            /* verification si des fils sont termines */
            ret = joinDataThread();
            if (ret > 0) printf("server: %d thread terminé.\n", ret);        
        if (data == NULL) 
        {
            fflush(stdout);                                                 
        }                                                                        /////////////////////////////////////////////////////////////////////////////////////////////////

    if (argc != 2) 
    {                                                          
        erreur("usage: %s port\n", argv[0]);
    }

    mode = O_WRONLY|O_APPEND|O_CREAT;
    journal = open("journal.log", mode, 0660);
    if (journal == -1) 
    {
        erreur_IO("open journal");
    }

    port = (short) atoi(argv[1]);
  
    printf("server: creating a socket\n");
    ecoute = socket (AF_INET, SOCK_STREAM, 0);                                  // creation du socket d'ecoute de tentative de connexion 
    if (ecoute < 0) 
    {
        erreur_IO("socket");
    }
  
    adrEcoute.sin_family = AF_INET;
    adrEcoute.sin_addr.s_addr = INADDR_ANY;
    adrEcoute.sin_port = htons(port);
    printf("server: binding to INADDR_ANY address on port %d\n", port);
    ret = bind (ecoute,  (struct sockaddr *) &adrEcoute, sizeof(adrEcoute));
    if (ret < 0) 
    {
        erreur_IO("bind");
    }
  
    printf("server: listening to socket\n");
    ret = listen (ecoute, 20);
    if (ret < 0) 
    {
        erreur_IO("listen");
    }                                                                           /////////////////////////////////////////////////////////////////////////////////////////////////

    while (VRAI) 
    {
        printf("server: waiting to a connection\n");
        canal = accept(ecoute, (struct sockaddr *) &reception, &receptionlen);
        if (canal < 0) 
        {
            erreur_IO("accept");
        }
        printf("server: adr %s, port %hu\n",
	        stringIP(ntohl(reception.sin_addr.s_addr)),
	        ntohs(reception.sin_port));

        data = ajouterDataThread();
        if (data == NULL) 
        {
            erreur("allocation impossible\n");                                     // creation d'un worker traiterRequete lors d'une connexion d'un nouvel utilisateur
        }   
    
        data->spec.tid = ++numthread;
        data->spec.canal = canal;
        ret = pthread_create(&data->spec.id, NULL, traiterRequete, &data->spec);
        if (ret != 0) 
        {
            erreur_IO("pthread_create");
        }
        else 
        { /* thread main */
            printf("server: worker %d créé\n", numthread);
      
            /* verification si des fils sont termines */
            ret = joinDataThread();
            if (ret > 0) printf("server: %d thread terminé.\n", ret);
            fflush(stdout);
            continue;
        }
    }                                                                           ////////////////////////////////////////////////////////////////////////////////////////////////

    libererDataThread();   // on nettoie la ram
}
    exit(EXIT_SUCCESS);

}

void ajouterPseudo(char *texte, int tid)
{ // fonction qui ajoute un utilisateur à la liste globale avec le pid coreespondant et l'état "connecté"
    utilisateurs = realloc(utilisateurs, sizeof(utilisateurs[0])+1); //on augmente la taille du tableau quand un nouvel utilisateur se connecte
    strcpy(utilisateurs[taille].pseudo, texte);
    utilisateurs[taille].connecte = VRAI;
    utilisateurs[taille].flag = FAUX;
    strcpy(utilisateurs[taille].message, "");
    taille = taille + 1;
}

void deconnexion(int tid)    // la fonction ne supprime pas l'utilisateur mais passe son état à deconnecter
{ 
    utilisateurs[tid-1].connecte = FAUX; 
}

char *printTime(void)        // la fonction permet de retourner la date/heure sleon un format standard de time.h (utilité => log)
{ 

    time_t temps;
    struct tm date;

    time(&temps);
    date=*localtime(&temps);

    return asctime(&date); 
}

void ecrireLog(void)         //en tête pour les logs avec la date affichée
{ 
    char temps[35];

    sprintf(temps,"%s\n",printTime());

    if (ecrireLigne(journal, "--------------------------------------------------------------------------------\n") == -1) {
        erreur_IO("ecrireLigne");
    }
    if (ecrireLigne(journal, temps) == -1) {
        erreur_IO("ecrireLigne");
    }

}

void generateMdp(char* motDePasse) // fonction qui génére aléatoirement un mot de passe 256 bits à l'aide d'une liste de caractère
{
    char password[120]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890<>,?;.:/!§*µù$£¤¨+=})]à@ç^_`è|-[({'#é~&";
    int max=101,i,alea;
    for(i=0;i<32;i++)
    {       
        alea=rand()%(max);              // random/max de la liste
        motDePasse[i]=password[alea];
    } 
    motDePasse[33]='\0';
}

int demandeEnvoi(int id_emetteur, int id_receveur)
{
    utilisateurs[id_receveur-1].flag = 1;
    sprintf(utilisateurs[id_receveur-1].message,"%d", id_emetteur);
    while(utilisateurs[id_receveur-1].flag == 1);
    if(strcmp(utilisateurs[id_receveur-1].message, "OK")==0)
    {
        return VRAI;
    }
    else
    {
        return FAUX;
    }
}

int idValide(char * id)
{
    int id_nombre;
    id_nombre = atoi(id);
    if(id_nombre > taille && id_nombre <= 0)
    {
        return FAUX;
    }
    else
    {
        if(utilisateurs[id_nombre-1].connecte && utilisateurs[id_nombre-1].flag == 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

void envoiMDP(char * recepteur, char * motDePasse)
{
    int recep = atoi(recepteur)-1;
    strcpy(utilisateurs[recep].message, motDePasse);
    utilisateurs[recep].flag = 1;
}