/*#ifndef CLIENT_H
#define CLIENT_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "../objects/player.h"

typedef struct {
    int clientId;
    SDLNet_SocketSet set;
    UDPsocket udpSocket;
    UDPpacket *pPacket;
    Player *players[4];
} Client;

//TCP
Client *createClient();
int receiveFromServer(Client *pClient, Player *player);
void closeClient(Client *pClient);
int sendDataUDP(Client *pClient, Player *player);

#endif*/