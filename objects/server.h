#ifndef SERVER_H
#define SERVER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

typedef struct {
    TCPsocket serverSocket;
	UDPpacket *pPacket;
    int nrOfClients;
} Server;

Server createServer();
int acceptClientConnections(Server *pServer);
int sendMessageToClient(TCPsocket clientSocket); 
//void handleClientData(Client *client);
//void listenForClientData();
//void sendGameData();

#endif