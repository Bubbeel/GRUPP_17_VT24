#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/server.h"
#include "../objects/client.h"
#include "../objects/player.h"
#include "../objects/common.h"

#define PORT 3001

Server *createServer();
int acceptClientConnections(Server *pServer);
int sendMessageToClient(TCPsocket clientSocket);
int handleTCPData(char *receivedData, char *responseData);
int handleUDPData(char *receivedData, char *responseData);
void listenForClientData(Server *pServer, Client *pClient, Player *pPlayer);
void sendGameData(Server *pServer, Client *pClient, Player *player);

Server *createServer() {
    IPaddress ip;
    Server *pServer = malloc(sizeof(Server));

    if (SDLNet_ResolveHost(&ip, NULL, PORT) == -1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        pServer->serverSocket=NULL;
        return pServer;
    }

    pServer->serverSocket = SDLNet_TCP_Open(&ip);
    if (!pServer->serverSocket) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return pServer;
    }

    return pServer;
}

int acceptClientConnections(Server *pServer) {
    TCPsocket clientSocket;
    const char *message = "You are connected";

    while (1) {
        clientSocket = SDLNet_TCP_Accept(pServer->serverSocket);
        if (clientSocket) {
            printf("Client connected!\n");

            // Send confirmation message to client
            if (sendMessageToClient(clientSocket) != 0) {
                SDLNet_TCP_Close(clientSocket);
                return 1;
            }

            // Close socket for client
            SDLNet_TCP_Close(clientSocket);
        }
    }

    return 0;
}

// Send a message to the connected client
int sendMessageToClient(TCPsocket clientSocket) {
    const char *message = "You are connected";
    int len = strlen(message) + 1;

    if (SDLNet_TCP_Send(clientSocket, message, len) < len) {
        fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        return 1;
    }

    return 0;
}
/*
void handleClientData(Client *pClient) {
    char data[512];
    if (SDLNet_TCP_Recv(pClient->clientSocket, data, sizeof(data)) > 0) {
        // Handle data
    }
}*/

int handleTCPData(char *receivedData, char *responseData){
    printf("Processing TCP data: %s\n", receivedData);

    strcpy(responseData, "TCP response");

    return 1;
}

int handleUDPData(char *receivedData, char *responseData){
    printf("Processing UDP data: %s\n", receivedData);

    strcpy(responseData, "UDP response");

    return 1;
}

void listenForClientData(Server *pServer, Client *pClient, Player *pPlayer) {
    UDPsocket udpSocket = SDLNet_UDP_Open(0);
    if(!udpSocket){
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return;
    }

    UDPpacket *pPacket = SDLNet_AllocPacket(512);
    if(!pPacket){
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(udpSocket);
        return;
    }

    while(1){
        char data[512];

        if(SDLNet_UDP_Recv(udpSocket, pPacket)>0){
            printf("Received UDP data from client: %s\n", (char *)pPacket->data);
            if(handleUDPData((char *)pPacket->data, data)){
                sendDataUDP(pClient, pPlayer);

            }
        }
    }

    SDLNet_UDP_Close(udpSocket);
    SDLNet_FreePacket(pPacket);
}

void sendGameData(Server *pServer, Client *pClient, Player *player){
    UDPsocket udpSocket = SDLNet_UDP_Open(0);
    if (!udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return;
    }

    UDPpacket *pPacket = SDLNet_AllocPacket(512);
    if(!pPacket){
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(udpSocket);
        return;
    }
    
    PlayerPackage pkg;
    pkg.clientId = pClient->clientId;
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.direction = player->direction;

    memcpy(pPacket->data, &pkg, sizeof(PlayerPackage));
    pPacket->len = sizeof(PlayerPackage);

    int channel = 0;
    for(int i=0;i<pServer->nrOfClients;i++){
        if (SDLNet_UDP_Send(udpSocket, channel, pPacket) == 0) {
            fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
        }
    }

    SDLNet_FreePacket(pPacket);
    SDLNet_UDP_Close(udpSocket);
}