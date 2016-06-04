#include "pse.h"

#define    NTHREADS       2
#define    MILLISECONDES  1000
#define    ATTENTE        2000*MILLISECONDES

/* donnees globales partagees par tous les threads */
DataSpec cohorte[NTHREADS];
sem_t sem_libres;
int journal;

/* traitement des requetes par les threads */
void *traiterRequete(void *arg) {
  DataSpec * data = (DataSpec *) arg;
  int arret = FAUX, nblus, mode;
  char texte[LIGNE_MAX];
  
  mode = O_WRONLY | O_APPEND | O_CREAT | O_TRUNC;

  while (VRAI) {
    printf("worker %d: attente canal.\n", data->tid);
    /* attente canal */
    if (sem_wait(&data->sem) != 0) {
      erreur_pthread_IO("sem_wait");
    }
    data->libre = FAUX;
    printf("worker %d: lecture canal %d.\n", data->tid, data->canal);

    arret = FAUX;
    while (arret == FAUX) {
      nblus = lireLigne (data->canal, texte);
      if (nblus <= 0 || nblus == LIGNE_MAX) {
	erreur("lireLigne\n");
      }
      
      if (strcmp(texte, "fin") == 0) {
	printf("worker %d: deconnexion demandee.\n", data->tid);
	arret = VRAI;
	continue;
      }
      
      if (strcmp(texte, "init") == 0) {
	printf("worker %d: remise a zero du journal demandee.\n", data->tid);
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
	printf("worker %d: ligne de %d octets ecrite dans le journal.\n",
	       data->tid, nblus);
	fflush(stdout);
      }
    }

    if (close(data->canal) == -1) {
      erreur_IO("close");
    }
    data->canal = -1;
    data->libre = VRAI;
    if (sem_post(&sem_libres) != 0) {
      erreur_pthread_IO("sem_post");
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int ecoute, canal, ret, mode, ilibre, i;
  struct sockaddr_in adrEcoute, reception;
  socklen_t receptionlen = sizeof(reception);
  short port;
  
  if (argc != 2) {
    erreur("usage: %s port\n", argv[0]);
  }

  mode = O_WRONLY|O_APPEND|O_CREAT;
  journal = open("journal.log", mode, 0660);
  if (journal == -1) {
    erreur_IO("open journal");
  }

  if (sem_init(&sem_libres, 0, NTHREADS) == -1) {
    erreur_IO("sem_init libres");
  }

  /* initialisation cohorte */
  for (i=0; i<NTHREADS; i++) {
    cohorte[i].tid = i;
    cohorte[i].libre = VRAI;
    /* une valeur -1 indique pas de requete a traiter */
    cohorte[i].canal = -1;
    if (sem_init(&cohorte[i].sem, 0, 0) == -1) {
      erreur_IO("sem_init");
    }
    ret = pthread_create(&cohorte[i].id, NULL, traiterRequete, &cohorte[i]);
    if (ret != 0) {
      erreur_IO("pthread_create");
    }
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

    if (sem_wait(&sem_libres) == -1) {
      erreur_IO("sem_post");
    }
    for (ilibre=0; ilibre<NTHREADS; ilibre++)
      if (cohorte[ilibre].libre) break;
    printf("serveur: %d\n", ilibre);

    cohorte[ilibre].canal = canal;
    if (sem_post(&cohorte[ilibre].sem) == -1) {
      erreur_IO("sem_post");
    }
    printf("server: worker %d choisi\n", ilibre);
      
  }
  
  exit(EXIT_SUCCESS);
}
