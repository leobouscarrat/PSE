#ifndef PROJET_INCLUDE_H
#define PROJET_INCLUDE_H

/*
Création d'une structure particuliere pour enregistrer les utilisateurs
*/

struct user{
	char pseudo[50];
	int connecte;
	int flag;
	char message[LIGNE_MAX];
}user;

#endif