#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/client.h"
#include "../objects/player.h"
#include "../objects/common.h"

#define SERVER_IP "127.0.0.1"
#define TCP_PORT 3001
#define UDP_PORT 3000

Client *createClient();
int connectToServer(Client *pClient);
int receiveFromServer(Client *pClient, Player *player);
void closeClient(Client *pClient);
void sendDataTCP(Client *pClient, char *data);
void sendDataUDP(Client *pClient, Player *Player);

Client *createClient(){
    Client *pClient=malloc(sizeof(Client));
    pClient->clientSocket=NULL;
    pClient->udpSocket=NULL;
    pClient->pPacket=NULL;
    return pClient;
}

int connectToServer(Client *pClient){
    //initialize SDL_Net (TCP and UDP)
    if(SDLNet_Init()<0){
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        return 1;
    }

    if(SDLNet_ResolveHost(&pClient->ip, SERVER_IP, TCP_PORT)==-1){
        fprintf(stderr, "SDLNet_ResloveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    pClient->clientSocket = SDLNet_TCP_Open(&pClient->ip);
    if(!pClient->clientSocket){
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    pClient->udpSocket = SDLNet_UDP_Open(0);
    if(!pClient->udpSocket){
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    pClient->pPacket = SDLNet_AllocPacket(512);
    if(!pClient->pPacket){
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_Quit();
        return 1;
    }
    return 0;
}

int receiveFromServer(Client *pClient, Player *player) {
    while (SDLNet_UDP_Recv(pClient->udpSocket, pClient->pPacket) > 0) {
        PlayerPackage *pkg = (PlayerPackage *)pClient->pPacket->data;

        player->playerX = pkg->x;
        player->playerY = pkg->y;
        player->direction = pkg->direction;

        printf("Received player data: X=%d, Y=%d, Direction=%d\n", player->playerX, player->playerY, player->direction);
    }
    fprintf(stderr, "No data received from server\n");
    return 1; 
}

void closeClient(Client *pClient){
    if(pClient->clientSocket){
        SDLNet_TCP_Close(pClient->clientSocket);
    }
    if(pClient->udpSocket){
        SDLNet_UDP_Close(pClient->udpSocket);
    }
    if(pClient->pPacket){
        SDLNet_FreePacket(pClient->pPacket);
    }
    SDLNet_Quit();
}


void sendDataUDP(Client *pClient, Player *player) {
    PlayerPackage pkg;
    pkg.clientId = pClient->clientId; 
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.direction = player->direction; 

    memcpy(pClient->pPacket->data, &pkg, sizeof(PlayerPackage));
    pClient->pPacket->len = sizeof(PlayerPackage);

    pClient->pPacket->address.host = pClient->ip.host;
    pClient->pPacket->address.port = UDP_PORT;
    SDLNet_UDP_Send(pClient->udpSocket, -1, pClient->pPacket);
}