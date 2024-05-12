#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/client.h"
#include "../objects/player.h"
#include "../objects/common.h"
#include "../objects/server.h"

#define SERVER_IP "127.0.0.1"
#define TCP_PORT 3000
#define UDP_PORT 3001

Client *createClient();
int connectToServer(Client *pClient);
int receiveFromServer(Client *pClient, Player *player);
void closeClient(Client *pClient);
void sendDataUDP(Client *pClient, Player *Player);

Client *createClient(){
    Server *pServer;
    Player *player;
    GameObject gameObject;
    IPaddress ip;
    SDL_Renderer *pRenderer = gameObject.renderer;
    Client *pClient=malloc(sizeof(Client));
    pClient->clientSocket=NULL;
    pClient->udpSocket=NULL;
    pClient->pPacket=NULL;
    for(int i = 0; i < 4; i++) {
        pClient->player[i]=malloc(sizeof(Player));
    }
    pClient->clientId = 0;
    player->playerId = pClient->clientId;
    pClient->clientId++;
    pClient->ip=ip;
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
    free(pClient);
    SDLNet_Quit();
}

int receiveFromServer(Client *pClient, Player *player) {
    if(SDLNet_UDP_Recv(pClient->udpSocket, pClient->pPacket) == 0){
        printf("correct socket");
    }
    if(SDLNet_UDP_Recv(pClient->udpSocket, pClient->pPacket) > 0) {
        PlayerPackage *pkg = (PlayerPackage *)pClient->pPacket->data;

        player->playerX = pkg->x;
        player->playerY = pkg->y;
        player->direction = pkg->direction;

        printf("Received player data: X=%d, Y=%d, Direction=%d\n", player->playerX, player->playerY, player->direction);
    }
    fprintf(stderr, "No data received from server\n");
    printf("udpsocket: %p\n", pClient->udpSocket);
    printf("packet: %p\n", pClient->pPacket);
    return 1; 
}


void sendDataUDP(Client *pClient, Player *player) {
    UDPpacket *packet = SDLNet_AllocPacket(512);
    if (!pClient || !player) {
        printf("Error: Null pointer\n");
        return;
    }

    PlayerPackage pkg;
    pkg.clientId = pClient->clientId; 
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.direction = player->direction; 

    memcpy(packet->data, &pkg, sizeof(PlayerPackage));

    packet->len = sizeof(PlayerPackage);

    packet->address.host = pClient->ip.host;
    packet->address.port = UDP_PORT;
    SDLNet_UDP_Send(pClient->udpSocket, -1, packet);
}