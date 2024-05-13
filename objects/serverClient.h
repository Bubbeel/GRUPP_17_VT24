#ifndef SERVER_H
#define SERVER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "../objects/player.h"
#include "../objects/common.h"

typedef struct {
    int clientId;
    SDLNet_SocketSet set;
    UDPsocket udpSocket;
    UDPpacket *pPacket;
    Player *players[4];
} Client;

typedef struct {
    int clientId;
    SDLNet_SocketSet set;
    Client *clients;
    UDPsocket udpSocket;
    UDPpacket *pPacket;
}Server;


Client *createClient();
int receiveFromServer(Client *pClient, Player *player);
void closeClient(Client *pClient);
int sendDataUDP(Client *pClient, Player *player);

Server *createServer();
void handlePlayerMovementData(char *data, Client *client, int *pNumClients, Server *pServer);
int listenForClientData(Server *pServer, Player *player);
void sendPlayerPosition(Client *pClient, PlayerMovementData *movementData, Server *pServer);
int acceptClientConnections(Server *pServer, int *pNumClients, Client *pClient);
void updateGame(Server *pServer, Client *pClient, Player *player);


#endif

