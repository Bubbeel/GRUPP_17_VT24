#ifndef SERVER_H
#define SERVER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "../objects/client.h"

typedef struct {
    TCPsocket serverSocket;
	UDPpacket *pPacket;
    int nrOfClients;
    Client clients[4];
}Server;

Server *createServer();
int acceptClientConnections(Server *pServer);
int sendMessageToClient(TCPsocket clientSocket); 
int handleTCPData(char *receivedData, char *responseData);
int handleUDPData(char *receivedData, char *responseData);
void listenForClientData(Server *pServer, Client *pClient, Player *pPlayer);
void sendGameData(Server *pServer, Client *pClient, Player *player);

#endif