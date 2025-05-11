#define _CRT_SECURE_NO_WARNINGS

#include "network.h"

void menuPrincipal(void)
{
	printf("\nQue souhaitez-vous faire?\n");
	printf("1. Lire\n");
	printf("2. Publier\n");
	printf("3. Modifier\n");
	printf("4. Supprimer\n");
	printf("0. Quitter\n");
	printf("Choix : ");
}

void recupererPseudo(USER* USE)
{
	printf("Entrez votre pseudo : ");
	fgets(USE->pseudo, sizeof(USE->pseudo), stdin);

	int pos = strcspn(USE->pseudo, "\n"); /*Trouver la position du 1er caractere de retour a la ligne '\n'*/
	USE->pseudo[pos] = '\0'; /*Remplacer retour a la ligne par caractere de fin de chaine*/
}

void recupererPassword(USER* USE)
{
	printf("Entrez votre mot de passe: ");
	fgets(USE->password, sizeof(USE->password), stdin);

	int pos = strcspn(USE->password, "\n");
	USE->password[pos] = '\0';
}

void registerClient(SOCKET clientSocket, SOCKADDR_IN serverUDP, USER* USE)
{
	char buffer[BUFFER_SIZE] = "";
	recupererPseudo(USE);

	do
	{
		strcpy(buffer, "REGISTER;");
		strcat(buffer, USE->pseudo);
		sendto(clientSocket, buffer, strlen(buffer), 0, (SOCKADDR*)&serverUDP, sizeof(serverUDP));
		printf("Message sent to server : %s\n", buffer);

		/*Reception de la reponse du serveur*/
		unsigned int serverUDP_Len = sizeof(serverUDP);
		ssize_t recvBytes = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (SOCKADDR*)&serverUDP, &serverUDP_Len);

		if (recvBytes < 0)
		{
			perror("Recvfrom failed ");
			exit(EXIT_FAILURE);
		}

		buffer[recvBytes] = '\0';

		printf("Message recv from server : %s\n", buffer);

		if (strcmp(USE->pseudo, buffer) == 0)
		{
			printf("Pseudo deja pris; choisi un autre\n");
			recupererPseudo(USE);
		}

		if (strcmp("NULL", buffer) == 0)
			break;
	} while (strcmp(USE->pseudo, buffer) == 0);

	recupererPassword(USE);
}

void connectClient(SOCKET clientSocket, SOCKADDR_IN serverUDP, USER* USE)
{
	char buffer[BUFFER_SIZE] = "";
	recupererPseudo(USE);
	recupererPassword(USE);

	do
	{
		/*Verifier si le client existe ou non*/
		strcpy(buffer, "CONNECT;");
		strcat(buffer, USE->pseudo);
		strcat(buffer, ";");
		strcat(buffer, USE->password);

		sendto(clientSocket, buffer, strlen(buffer), 0, (SOCKADDR*)&serverUDP, sizeof(serverUDP));
		printf("Message sent to server : %s\n", buffer);

		/*Reception de la reponse du serveur*/
		unsigned int serverUDP_Len = sizeof(serverUDP);
		ssize_t recvBytes = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (SOCKADDR*)&serverUDP, &serverUDP_Len);

		if (recvBytes < 0)
		{
			perror("Recvfrom failed ");
			exit(EXIT_FAILURE);
		}

		buffer[recvBytes] = '\0';

		printf("Message recv from server : %s\n", buffer);

		if (strcmp("NULL", buffer) == 0)
		{
			printf("Pseudo ou mot de passe incorrect\n");
			recupererPseudo(USE);
			recupererPassword(USE);
		}
	} while (strcmp(USE->pseudo, buffer) != 0);
}

/* Fonction pour initialiser une pile vide */
struct Stack* createStack(void)
{
	struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));

	if (stack == NULL)
	{
		printf("Erreur lors de l'allocation de memoire pour la pile.\n");
		exit(1);
	}
	stack->top = NULL;
	return stack;
}