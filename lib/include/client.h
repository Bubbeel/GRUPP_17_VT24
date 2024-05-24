/*
#ifndef CLIENT_H
#define CLIENT_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "../objects/player.h"
#include "../objects/common.h"
#include "../objects/server.h"

typedef struct {
    UDPsocket udpSocket;
    TCPsocket clientSocket;
    IPaddress ip;
    UDPpacket *pPacket;
    Player *players[4];
    int nrOfPlayers;
    int playerNr;
} Client;

//TCP

Client *createClient();
int connectToServer(Client *pClient);
int receiveFromServer(Client *pClient, Player *player, UDPpacket *packet, UDPsocket udpSocket);
void closeClient(Client *pClient);
void sendDataUDP(Client *pClient, Player *player, UDPpacket *packet, UDPsocket udpSocket);

#endif*/