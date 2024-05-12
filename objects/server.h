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
    Client clients[4];
    Player *player[4];
}Server;

Server *createServer();
int acceptClientConnections(Server *pServer, int *pNrOfClients, Client *client);
int sendMessageToClient(TCPsocket clientSocket);
void handlePlayerMovementData(char *data, Client *client, int *pNrOfClients, Server *pServer);
void listenForClientData(Server *pServer, Client *pClients, int *pNrOfClients);
void sendGameData(Server *pServer, Client *pClient, Player *player);
void sendPlayerPosition(Client *client, Player *player);

#endif