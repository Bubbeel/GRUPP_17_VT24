#ifndef SERVER_H
#define SERVER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "../objects/player.h"
#include "../objects/common.h"
#include "../objects/gridmap.h"

typedef struct {
    UDPsocket udpSocket;
    UDPpacket *pPacket;
    Player *players[4];
    int nrOfPlayers;
    int playerNr;
} Client;

typedef struct {
    int clientId;
    int nrOfPlayers;
    UDPsocket udpSocket;
    UDPpacket *pPacket;
    Player *players[4];
    IPaddress clients[4];
    int nrOfClients;
}Server;


Client *createClient(Client *pClient,SDL_Renderer *pRenderer, GridMap *map, int nrOfClients);
int receiveFromServer(Client *pClient, Player *player, Server *pServer);
void closeClient(Client *pClient);
int sendDataUDP(Client *pClient, Player *player, Server *pServer);

Server *createServer(Server *pServer,SDL_Renderer *pRenderer, GridMap *map);
void handlePlayerMovementData(char *data, Client *client, int *pNumClients, Server *pServer);
int listenForClientData(Server *pServer, Player *player, Client *pClient);
void sendPlayerPosition(Client *pClient, PlayerMovementData *movementData, Server *pServer);
void add(IPaddress address, IPaddress clients[], Server *pServer);
int waitForClients(Server *pServer, int clientIndex, SDL_Renderer *pRenderer, GridMap *map);
void handleClientConnection(Server *pServer, int clientIndex, SDL_Renderer *pRenderer, GridMap *map);


#endif