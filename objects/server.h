#ifndef SERVER_H
#define SERVER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "../objects/client.h"

typedef struct {
    TCPsocket serverSocket;
    UDPsocket udpSocket;
	UDPpacket *pPacket;
    Client clients[4];
    Player *player[4];
}Server;

Server *createServer();
int acceptClientConnections(Server *pServer, int *pNumClients, Client *client);
int sendMessageToClient(TCPsocket clientSocket);
void handlePlayerMovementData(char *data, Client *client, int *pNumClients, Server *pServer);
void listenForClientData(Server *pServer, Client *pClients, int *pNumClients, UDPsocket udpSocket, UDPpacket *packet);
void sendPlayerPosition(Server *pServer, Client *client, Player *player, UDPsocket udpSocket, UDPpacket *packet);
#endif