#ifndef NETWORK_H
#define NETWORK_H
#define SIZE_MESSAGE 100
#define SIZE_USER 20
#define BUFFER_SIZE 1024
#define DEFAULT_PORT 8080
//#define WIN32

#ifdef _WIN32
#pragma warning(disable: 4668 4820 4710)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSDKVer.h> 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <synchapi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#pragma warning(default: 4668 4820 4710)
#define sleep Sleep

#pragma comment(lib, "Ws2_32.lib")

typedef long int ssize_t;
typedef unsigned short int uint16_t;

#else
#ifdef __linux__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /*strcpy...*/
#include <sys/time.h>
#include <pthread.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

//#error not available on your platform

#endif
#endif

static void initNetwork(void)
{
#ifdef _WIN32
	WSADATA wsa;
	int err = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (err < 0)
	{
		printf("WSAStartup failed !\n");
		exit(EXIT_FAILURE);
	}
#endif
}

static void cleanNetwork(void)
{
#ifdef _WIN32
	WSACleanup();
#endif
}
#endif

/* Structure pour stocker les donnees du client*/
typedef struct ClientData
{
	char pseudo[SIZE_USER];
	char donnees[SIZE_MESSAGE];
	char password[SIZE_USER];
	int ID_Message;
	struct ClientData* next;
}USER;
/* Structure pour la liste chainee LIFO */
struct Stack
{
	struct ClientData* top;
};


/**********************************************************/
void clientUDP(int PORT);
void serverUDP(void);
void sendMessageToServer(SOCKET clientSocket, SOCKADDR_IN serverUDP, const char* key, const char* user, const char* message);
void recvMessageFromServer(SOCKET clientSocket, SOCKADDR_IN serverUDP);

void sendMessageToClient(SOCKET serverSocket, SOCKADDR_IN clientUDP, const char* message);
void sendResponse(SOCKET serverSocket, SOCKADDR_IN clientUDP, const char* pseudo, const char* donnees);

/*outils.c*/
void menuPrincipal(void);
void recupererPseudo(USER* USE);
void recupererPassword(USER* USE);

void registerClient(SOCKET clientSocket, SOCKADDR_IN serverUDP, USER* USE);
void connectClient(SOCKET clientSocket, SOCKADDR_IN serverUDP, USER* USE);

struct Stack* createStack(void);
/**********************************************************/

