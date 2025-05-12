#define _CRT_SECURE_NO_WARNINGS

#include "network.h"

int main(void)
{
	initNetwork();

	serverUDP();

	cleanNetwork();
	return EXIT_SUCCESS;
}

/*Creation d'un serveur UDP*/
void serverUDP(void)
{
	/*Creation du socket*/
	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		perror("Socket cxreation failed !\n");
		exit(EXIT_FAILURE);
	}

	/*Configuration de l'adresse locale du serveur*/
	SOCKADDR_IN serverUDP = { 0 };
	serverUDP.sin_family = AF_INET;
	serverUDP.sin_port = htons(DEFAULT_PORT);
	serverUDP.sin_addr.s_addr = INADDR_ANY;

	/*Liaison du socket a l'adresse locale (bind socket to port)*/
	if (bind(serverSocket, (SOCKADDR*)&serverUDP, sizeof(serverUDP)) == SOCKET_ERROR)
	{
		perror("Bind failed");
		closesocket(serverSocket);
		exit(EXIT_FAILURE);
	}

	printf("*** UDP server ***\n");
	printf(" Creating new socket\n");
	printf("   Socket %d opened\n", serverSocket);
	printf("Server listening on port %d...\n\n", DEFAULT_PORT);

	/*Boucle pour recevoir les messages des clients*/
	char buffer[BUFFER_SIZE] = "";
	SOCKADDR_IN clientUDP = { 0 };
	unsigned int clientUDP_Len = sizeof(clientUDP);
	ssize_t recvBytes;
	struct Stack* stack = createStack(); /*Cree une pile vide*/

	while (1)
	{
		/*Reception du message du client*/
		recvBytes = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (SOCKADDR*)&clientUDP, &clientUDP_Len);

		if (recvBytes < 0)
		{
			perror("Recvfrom failed ");
			exit(EXIT_FAILURE);
		}

		buffer[recvBytes] = '\0';
		printf("Received from :%s:%d \n", inet_ntoa(clientUDP.sin_addr), ntohs(clientUDP.sin_port));
		printf("Message : %s\n", buffer);

		const char* delim = ";"; /*Delimiteur*/
		char* token; /* To store a token */
		char* str = buffer;
		static int nb_clients = 0;



		/* Get the first token */
		token = strtok(str, delim);
		if (strcmp(token, "REGISTER") == 0)
		{
			struct ClientData* current = stack->top;
			token = strtok(NULL, delim); /*Recuperer le pseudo fourni par le client*/
			
			while (current != NULL)
			{
				if (strcmp(current->pseudo, token) == 0) /*pseudo existe*/
				{
					sendMessageToClient(serverSocket, clientUDP, current->pseudo);
					break;
				}
				current = current->next;
			}

			if (current == NULL)
			{
				sendMessageToClient(serverSocket, clientUDP, "NULL");
				nb_clients++;
			}
		}
		else if (strcmp(token, "CONNECT") == 0)
		{
			struct ClientData* current = stack->top;
			token = strtok(NULL, delim); /*Recuperer le pseudo fourni par le client*/

			while (current != NULL)
			{
				if (strcmp(current->pseudo, token) == 0) /*Pseudo trouve*/
				{
					printf("Pseudo\n");
					token = strtok(NULL, delim); /* On recupere le password */
					int pos = strcspn(token, "\n");
					token[pos] = '\0';

					if (strcmp(current->password, token) == 0) /*Mot de passe correct*/
					{
						sendMessageToClient(serverSocket, clientUDP, current->pseudo);
						nb_clients++;
						break;
					}
				}
				current = current->next;
			}

			if(current == NULL)
				sendMessageToClient(serverSocket, clientUDP, "NULL");
		}
		else  if (strcmp(token, "PUSH") == 0) /*Stocker*/
		{
			struct ClientData* newNode = (struct ClientData*)malloc(sizeof(struct ClientData));
			static int message_counter = 0;

			if (newNode == NULL)
			{
				printf("Erreur lors de l'allocation de memoire pour le nouveau client.\n");
				exit(1);
			}

			/* On recupere le pseudo */
			token = strtok(NULL, delim);
			strcpy(newNode->pseudo, token);

			/* On recupere le mot de passe */
			token = strtok(NULL, delim);
			strcpy(newNode->password, token);

			/* On recupere le message a stocker pour le client*/
			token = strtok(NULL, delim);
			strcpy(newNode->donnees, token);

			/*On attribue un id unique au message en utilisant un compteur statique*/
			newNode->ID_Message = message_counter++;

			newNode->next = stack->top;
			stack->top = newNode;
			printf("Message stocke\n");
		}
		else  if (strcmp(token, "PULL") == 0) /*Afficher pour le client*/
		{
			token = strtok(NULL, delim); /*On recupere le pseudo*/

			if (strcmp(token, "vide") == 0) /*Pour tous les utilisateurs*/
			{
				token = strtok(NULL, delim); /*On recupere le nombre de message a lire*/

				if (strcmp(token, "vide") == 0) /*Tous les messages*/
				{
					/*Serveur envoi un message au client pour lui dire qu'il va afficher tous les messages de tous les utilisateurs*/
					system("cls");
					sendMessageToClient(serverSocket, clientUDP, "All messages will be read\n\n");

					/*On va afficher tous les messages de tous les utilisateurs pour le client*/
					struct ClientData* current = stack->top;
					char response[BUFFER_SIZE] = "";

					while (current != NULL)
					{
						sendResponse(serverSocket, clientUDP, current->pseudo, current->donnees);
						current = current->next;
					}
					sendMessageToClient(serverSocket, clientUDP, "END_OF_TRANSMISSION");
				}
				else /*N messages*/
				{
					int i = 0; /*Iteration variable*/
					int N; /*Nombre de message a afficher pour le client*/
					struct ClientData* current = stack->top;

					N = atoi(token); /*Le nombre de message a lire*/

					int pos = strcspn(token, "\n"); /*Trouver la position du 1er caractere de retour a la ligne '\n'*/
				  token[pos] = '\0'; /*Remplacer retour a la ligne par caractere de fin de chaine*/

					/*Serveur envoi un message au client pour lui dire qu'il va afficher les N derniers messages de tous les utilisateurs*/
					char response[BUFFER_SIZE] = "";
					system("cls");
					sprintf(response, "%d messages will be read\n\n", N);
					sendMessageToClient(serverSocket, clientUDP, response);

					/*On va afficher les N derniers messages*/
					while (current != NULL && i < N)
					{
						sendResponse(serverSocket, clientUDP, current->pseudo, current->donnees);
						current = current->next;
						i++;
					}
					sendMessageToClient(serverSocket, clientUDP, "END_OF_TRANSMISSION");
				}
			}
			else /*not all user*/
			{
				struct ClientData* current = stack->top;
				int c = 0;

				while (current != NULL)
				{
					if (strcmp(current->pseudo, token) == 0)
					{
						c++;
						/*On recupere le nombre de message a lire*/
						token = strtok(NULL, delim);

						if (strcmp(token, "vide") == 0) /*Tous les messages*/
						{
							/*Serveur envoi un message au client pour lui dire qu'il va afficher tous les messages du pseudo en question*/
							char response[BUFFER_SIZE] = "";

							system("cls");
							sendMessageToClient(serverSocket, clientUDP, "All messages will be read\n\n");

							/*On va afficher tous les messages du pseudo*/
							while (current != NULL)
							{
								sendResponse(serverSocket, clientUDP, current->pseudo, current->donnees);
								current = current->next;
							}
							sendMessageToClient(serverSocket, clientUDP, "END_OF_TRANSMISSION");
						}
						else /*N messages*/
						{
							int i = 0; /*Iteration variable*/
							int N; /*Nombre de message a afficher pour le client*/

							N = atoi(token); /*Le nombre de message a lire*/
							int pos = strcspn(token, "\n"); /*Trouver la position du 1er caractere de retour a la ligne '\n'*/
							token[pos] = '\0'; /*Remplacer retour a la ligne par caractere de fin de chaine*/

							/*Serveur envoi un message au client pour lui dire qu'il va afficher les N derniers messages de tous les utilisateurs*/
							char response[BUFFER_SIZE] = "";

							system("cls");
							sprintf(response, "%d messages will be read\n\n", N);
							sendMessageToClient(serverSocket, clientUDP, response);

							/*On va afficher les N derniers messages*/
							while (current != NULL && i < N)
							{
								sendResponse(serverSocket, clientUDP, current->pseudo, current->donnees);
								current = current->next;
								i++;
							}
							sendMessageToClient(serverSocket, clientUDP, "END_OF_TRANSMISSION");
						}
					}

					if (c == 1)
						break;
					current = current->next;
				}
				if (c == 0) /*Dire au client qu'on n'a pas trouve le pseudo*/
				{
					system("cls");
					sendMessageToClient(serverSocket, clientUDP, "Pseudo non trouve");
					sendMessageToClient(serverSocket, clientUDP, "END_OF_TRANSMISSION");
				}
			}
		}
		else if (strcmp(token, "MODIFY") == 0)
		{
			/* Envoi de la liste des messages avec leurs ID correspondants */
				struct ClientData* current = stack->top;
			token = strtok(NULL, delim); /*recuperer le pseudo fourni par le client */

				while (current != NULL)
				{
					if (strcmp(current->pseudo, token) == 0)
					{
						/* Serveur envoi un message au client pour lui dire qu'il va afficher tous les messages du pseudo en question*/
							char response[BUFFER_SIZE] = "";
						sprintf(response, "%d : %s", current->ID_Message, current->donnees);
						sendMessageToClient(serverSocket, clientUDP, response);
					}
					current = current->next;
				}
			sendMessageToClient(serverSocket, clientUDP, "END_OF_TRANSMISSION");

			/* Reception de l'ID du message a modifier et le nouveau message*/
			int id;
			char str2[BUFFER_SIZE] =	 "";
			recvfrom(serverSocket, str2, sizeof(str2), 0, (SOCKADDR*)&clientUDP, &clientUDP_Len);
			id = atoi(strtok(str2, delim));

			char * newMessage = strtok(NULL, delim); /* Recuperer le nouveau message fourni par le client */

			struct ClientData* prev = NULL;
			current = stack->top;

			while (current != NULL)
			{
				if (current->ID_Message == id)
				{
					if (prev == NULL) /* Le message a modifier est au debut de la liste */
						strcpy(current->donnees, newMessage);
					else
						strcpy(current->donnees, newMessage);

					sendMessageToClient(serverSocket, clientUDP, "Le message a ete modifie avec succes");
					break;
				}

				prev = current;
				current = current->next;

				if (current == NULL)
					sendMessageToClient(serverSocket, clientUDP, "Aucun message trouve avec l'id specifie");
			}
			sendMessageToClient(serverSocket, clientUDP, "END_OF_TRANSMISSION");
		}
		else if (strcmp(token, "DELETE") == 0)
		{
			/*Envoi de la liste des messages avec leurs ID correspondants*/
			struct ClientData* current = stack->top;
			token = strtok(NULL, delim); /*Recuperer le pseudo fourni par le client*/

			while (current != NULL)
			{
				if (strcmp(current->pseudo, token) == 0)
				{
					/*Serveur envoi un message au client pour lui dire qu'il va afficher tous les messages du pseudo en question*/
					char response[BUFFER_SIZE] = "";
					sprintf(response, "%d : %s", current->ID_Message, current->donnees);
					sendMessageToClient(serverSocket, clientUDP, response);
				}
				current = current->next;
			}
			sendMessageToClient(serverSocket, clientUDP, "END_OF_TRANSMISSION");

			/*Reception de l'ID du message a supprimer*/
			char chosenID[3] = "";
			recvfrom(serverSocket, chosenID, sizeof(chosenID), 0, (SOCKADDR*)&clientUDP, &clientUDP_Len);

			int id = atoi(chosenID);
			struct ClientData* prev = NULL;
			current = stack->top;

			while (current != NULL)
			{
				if (current->ID_Message == id)
				{
					if (prev == NULL) /*Le message a supprimer est au debut de la liste*/
						stack->top = current->next;
					else
						prev->next = current->next;

					sendMessageToClient(serverSocket, clientUDP, "Le message a ete supprime avec succes");
					break;
				}

				prev = current;
				current = current->next;

				if (current == NULL)
					sendMessageToClient(serverSocket, clientUDP, "Aucun message trouve avec l'id specifie");
			}
			sendMessageToClient(serverSocket, clientUDP, "END_OF_TRANSMISSION");
		}

		/*Check if client wants to quit*/
		if (strcmp(buffer, "SERVER:QUIT") == 0)
		{
			system("cls");
			if (nb_clients > 0)
			{
				printf("\nClient number %d has quit.\n", nb_clients);
				nb_clients--;
			}
			
			if (nb_clients == 0)
			{
				printf("Aucun client n'est connecte au serveur.\nEn attente de connexion...\n");
				printf("Pressez enter pour rester en attente ou 0 pour arreter le serveur...\n");
				/*
					Puisque le on invite l'utilisateur a entrer 0 pour arreter le serveur
					il doit forcement effectuer une action (d'ou le presser enter) car le serveur attend une donnee
					en utilisant fgets(...)
				*/
				int operation = 1;
				char choix[3];
				fgets(choix, sizeof(choix), stdin);
				sscanf(choix, "%d", &operation);

				if (operation == 0)
					break;
			}

		}

		memset(buffer, 0, BUFFER_SIZE); /*Reset buffer*/
	}

	closesocket(serverSocket); /*Fermeture du socket*/
}

/*Envoie un message formate au client*/
void sendMessageToClient(SOCKET serverSocket, SOCKADDR_IN clientUDP, const char* message)
{
	sendto(serverSocket, message, strlen(message), 0, (SOCKADDR*)&clientUDP, sizeof(clientUDP));
	printf("Message sent to client : %s\n", message);
}

void sendResponse(SOCKET serverSocket, SOCKADDR_IN clientUDP, const char* pseudo, const char* donnees)
{
	char response[BUFFER_SIZE] = "";

	strcat(response, "@");
	strcat(response, pseudo);
	strcat(response, " : ");
	strcat(response, donnees);
	strcat(response, "\n");
	sendMessageToClient(serverSocket, clientUDP, response);
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