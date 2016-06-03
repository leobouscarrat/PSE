#include "pse.h"

int journal;
int *utilisateurs;


void *traiterRequete(void *arg);

void *traiterRequete(void *arg) {
    DataSpec * data = (DataSpec *) arg;
    int arret = FAUX, nblus, mode, pseudo = FAUX;
    char texte[LIGNE_MAX];
  
    mode = O_WRONLY | O_APPEND | O_CREAT | O_TRUNC;

    /*Dans un premier temps nous allons enregistrer le pseudo et voir
    si il n'existe pas déjà*/
    while (pseudo == FAUX){
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
        }
    }
    while (arret == FAUX) {
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
            if (strcmp(texte, "fin") == 0) {
	           printf("worker%d: arret demande.\n", data->tid);
	           arret = VRAI;
	           continue;
            }
            else if (strcmp(texte, "init") == 0) {
	           printf("worker%d: remise a zero du journal demandee.\n", data->tid);
	           if (close(journal) == -1) {
	               erreur_IO("close journal");
	            }
	            journal = open("journal.log", mode, 0660);
	            if (journal == -1) {
	               erreur_IO("open trunc journal");
	            }
            }
            else {
	            if (ecrireLigne(journal, texte) == -1) {
	                erreur_IO("ecrireLigne");
	            }
	            printf("worker%d: ligne de %d octets ecrite dans le journal.\n", 
	            data->tid, nblus);
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
