#define _CRT_SECURE_NO_WARNINGS

#include "network.h"

int main(int argc, char* argv[])
{
	initNetwork();
	int port = DEFAULT_PORT; /*Port par defaut*/

	if (argc > 1)
		port = atoi(argv[1]); /*Si un port est fourni en argument, on l'utilise*/

	clientUDP(port);

	cleanNetwork();
	return EXIT_SUCCESS;
}

/*Creation d'un client UDP*/
void clientUDP(int PORT)
{
	char choix[7];
	int operation; 
	USER USE; /*Utilisateur*/
	int pos;

	SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		perror("Socket cxreation failed !\n");
		exit(EXIT_FAILURE);
	}

	/*Configuration de l'adresse du serveur*/
	SOCKADDR_IN serverUDP = { 0 };
	serverUDP.sin_family = AF_INET;
	serverUDP.sin_port = htons(PORT);
	serverUDP.sin_addr.s_addr = inet_addr("127.0.0.1");

	printf("*** UDP client ***\n");
	printf(" Creating new socket\n");
	printf("   Socket %d opened\n", clientSocket);
	printf("Trying to send data on port %d...\n\n", PORT);


	printf("1. Premiere connextion ? \n");
	printf("2. Se connecter ? \n");
	printf("Choix : ");
	fgets(choix, sizeof(choix), stdin);
	sscanf(choix, "%d", &operation);
	system("cls");
	
	if (operation == 1)
		registerClient(clientSocket, serverUDP, &USE);
	else if (operation == 2)
		connectClient(clientSocket, serverUDP, &USE);
	system("cls");

	do
	{
		menuPrincipal();
		fgets(choix, sizeof(choix), stdin);
		sscanf(choix, "%d", &operation);
		system("cls");

		if (operation == 2) /*publication de message*/
		{
			printf("\nEntrez le message a publier : ");
			fgets(USE.donnees, sizeof(USE.donnees), stdin);

			/*Envoie du message au serveur*/
			sendMessageToServer(clientSocket, serverUDP, "PUSH", USE.pseudo, USE.password, USE.donnees);
		}
		else if (operation == 1) /*Lecture de message*/
		{
			printf("\nQue souhaitez-vous?\n");
			printf("1. Acceder aux messages de tous les utilisateurs\n");
			printf("0. Acceder aux messages d'un utilisateur particulier\n");
			printf("Choix : ");
			fgets(choix, sizeof(choix), stdin);
			sscanf(choix, "%d", &operation);
			system("cls");

			if (operation) /*All user*/
			{
				printf("Entrez 1 si vous voulez acceder a tous les messages et 0 sinon : ");
				fgets(choix, sizeof(choix), stdin);
				sscanf(choix, "%d", &operation);

				if (operation) /*All message*/
				{
					/*Envoie du message au serveur*/
					sendMessageToServer(clientSocket, serverUDP, "PULL", "", "vide", "vide");

					/*Reception de la reponse du serveur*/
					system("cls");
					recvMessageFromServer(clientSocket, serverUDP);
				}
				else /*N messages*/
				{
					printf("Entrez le nombre de message que vous voulez afficher : ");
					fgets(choix, sizeof(choix), stdin);

					/*Envoie du message au serveur*/
					sendMessageToServer(clientSocket, serverUDP, "PULL", "", "vide", choix);

					/*Reception de la reponse du serveur*/
					system("cls");
					recvMessageFromServer(clientSocket, serverUDP);

				}
			}
			else /*Un user particulier*/
			{
				char pseudo[SIZE_USER];

				printf("Entrez le pseudo de l'utilisateurs : ");
				fgets(pseudo, sizeof(pseudo), stdin);
				pos = strcspn(pseudo, "\n"); /*Trouver la position du 1er caractere de retour a la ligne '\n'*/
				pseudo[pos] = '\0'; /*Remplacer retour a la ligne par caractere de fin de chaine*/

				printf("Entrez 1 si vous voulez acceder a tous les messages et 0 sinon : ");
				fgets(choix, sizeof(choix), stdin);
				sscanf(choix, "%d", &operation);

				if (operation == 1) /*All message*/
				{
					/*Envoie du message au serveur*/
					sendMessageToServer(clientSocket, serverUDP, "PULL", pseudo, "", "vide");

					/*Reception de la reponse du serveur*/
					system("cls");
					recvMessageFromServer(clientSocket, serverUDP);
				}
				else /*N messages*/
				{
					printf("Entrez le nombre de message que vous vcoulez afficher : ");
					fgets(choix, sizeof(choix), stdin);

					/*Envoie du message au serveur*/
					sendMessageToServer(clientSocket, serverUDP, "PULL", pseudo, "", choix);

					/*Reception de la reponse du serveur*/
					system("cls");
					recvMessageFromServer(clientSocket, serverUDP);
				}
			}
		}
		else if (operation == 3) /* Modifier un message */
		{
			/* Modifier un message */

				sendMessageToServer(clientSocket, serverUDP, "MODIFY", USE.pseudo, "", ""); /* Envoi de la commande MODIFY au serveur */

				recvMessageFromServer(clientSocket, serverUDP); /* Reception de la liste des messages avec leurs id correspondants */
				
				printf("\nChoisissez l'ID du message que vous voulez modifier : ");
				fgets(choix, sizeof(choix), stdin);
				int pos = strcspn(choix, "\n"); /*Trouver la position du 1er caractere de retour a la ligne '\n'*/
				choix[pos] = '\0'; /*Remplacer retour a la ligne par caractere de fin de chaine*/

			  /* Demander au client le nouveau message */
				char newMessage[BUFFER_SIZE] = "";
				char message[BUFFER_SIZE] = "";

				system("cls");
		  	printf("\nEntrez le nouveau message : ");
			  fgets(newMessage, sizeof(newMessage), stdin);
			  pos = strcspn(newMessage, "\n"); /*Trouver la position du 1er caractere de retour a la ligne '\n'*/
				newMessage[pos] = '\0'; /*Remplacer retour a la ligne par caractere de fin de chaine*/

			 /* Envoyer l'identifiant du message a modifier et le nouveau message au serveur */
				strcat(message, choix);
				strcat(message, ";");
				strcat(message, newMessage);
			  sendMessageToServer(clientSocket, serverUDP, message, "", "", ""); /* Envoi de la commande MODIFY au serveur*/

				system("cls");
			/*Reception de la confirmation de reception du serveur */
			 recvMessageFromServer(clientSocket, serverUDP);
		}
		else if (operation == 4) /*Supprimer un message*/
		{
			/*Supprimer un message*/

			sendMessageToServer(clientSocket, serverUDP, "DELETE", USE.pseudo, "", ""); /*Envoi de la commande DELETE au serveur*/

			recvMessageFromServer(clientSocket, serverUDP); /*Reception de la liste des messages avec leurs id correspondants*/
			printf("\nChoisissez l'ID du message que vous voulez supprimer : ");
			fgets(choix, sizeof(choix), stdin);
			sscanf(choix, "%d", &operation);

			/*Envoi de l'id du message a supprimer au serveur*/
			sprintf(choix, "%d", operation);
			sendMessageToServer(clientSocket, serverUDP, choix, "", "", ""); /*Envoi de la commande DELETE au serveur*/

			/*Reception de la confirmation de reception du serveur*/
			recvMessageFromServer(clientSocket, serverUDP);
		}
		else if (operation == 0)
		{
			/*Envoie du message du message SERVER:QUIT au serveur pour arreter*/
			char message[] = "SERVER:QUIT";
			sendto(clientSocket, message, strlen(message), 0, (SOCKADDR*)&serverUDP, sizeof(serverUDP));
			printf("Message sent to server : %s\n", message);
			break;
		}
	} while (1);

	closesocket(clientSocket); /*Fermeture du socket*/
}

/*Envoie un message formate au serveur*/
void sendMessageToServer(SOCKET clientSocket, SOCKADDR_IN serverUDP, const char* key, const char* user, const char* password, const char* message)
{
	char buffer[BUFFER_SIZE + 8] = "";
	int length;

	length = snprintf(buffer, BUFFER_SIZE, "%s;%s;%s;%s", key, user, password, message);

	if (length >= BUFFER_SIZE)
	{
		printf("Erreur : le message est trop long.\n");
		exit(EXIT_FAILURE);
	}
	sendto(clientSocket, buffer, strlen(buffer), 0, (SOCKADDR*)&serverUDP, sizeof(serverUDP));
	printf("Message sent to server : %s\n", buffer);
}

/*Reception de la reponse du serveur*/
void recvMessageFromServer(SOCKET clientSocket, SOCKADDR_IN serverUDP)
{
	char buffer[BUFFER_SIZE] = "";
	unsigned int serverUDP_Len = sizeof(serverUDP);
	
	while (1)
	{
		ssize_t recvBytes = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (SOCKADDR*)&serverUDP, &serverUDP_Len);

		if (recvBytes < 0)
		{
			perror("Recvfrom failed ");
			exit(EXIT_FAILURE);
		}
		
		buffer[recvBytes] = '\0';

		if (strcmp(buffer, "END_OF_TRANSMISSION") == 0)
			break;

		printf("%s", buffer);
	}
}