/*#include <stdio.h>
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
int receiveFromServer(Client *pClient, Player *player, UDPpacket *packet, UDPsocket udpSocket);
void closeClient(Client *pClient);
void sendDataUDP(Client *pClient, Player *player, UDPpacket *packet, UDPsocket udpSocket);

Client *createClient() {
    Client *pClient = malloc(sizeof(Client));
    if (!pClient) {
        fprintf(stderr, "Error: Failed to allocate memory for client\n");
        return NULL;
    }

    if (SDLNet_ResolveHost(&pClient->ip, SERVER_IP, TCP_PORT) == -1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        free(pClient);
        return NULL;
    }

    pClient->clientSocket = SDLNet_TCP_Open(&pClient->ip);
    printf("clientSocket: %p\n", pClient->clientSocket);
    if (!pClient->clientSocket) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        free(pClient);
        return NULL;
    }

    pClient->udpSocket = SDLNet_UDP_Open(0);
    printf("udpsocket: %p\n", pClient->udpSocket);
    if (!pClient->udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(pClient->clientSocket);
        free(pClient);
        return NULL;
    }

    pClient->pPacket = SDLNet_AllocPacket(512);
    printf("packet: %p\n", pClient->pPacket);
    if (!pClient->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_TCP_Close(pClient->clientSocket);
        free(pClient);
        return NULL;
    }

    for (int i = 0; i < 4; i++) {
        pClient->players[i] = malloc(sizeof(Player));
        if (!pClient->players[i]) {
            fprintf(stderr, "Error: Failed to allocate memory for player %d\n", i);
            SDLNet_FreePacket(pClient->pPacket);
            SDLNet_UDP_Close(pClient->udpSocket);
            SDLNet_TCP_Close(pClient->clientSocket);
            free(pClient);
            return NULL;
        }
    }

    return pClient;
}

int connectToServer(Client *pClient){
    //initialize SDL_Net (TCP and UDP)
    if(!pClient->clientSocket){
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }
    if(!pClient->udpSocket){
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }
    if(!pClient->pPacket){
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_Quit();
        return 1;
    }
    return 0;
}

void closeClient(Client *pClient) {
    if (pClient) {
        if (pClient->clientSocket) {
            SDLNet_TCP_Close(pClient->clientSocket);
        }
        if (pClient->udpSocket) {
            SDLNet_UDP_Close(pClient->udpSocket);
        }
        if (pClient->pPacket) {
            SDLNet_FreePacket(pClient->pPacket);
        }
        for (int i = 0; i < 4; i++) {
            if (pClient->players[i]) {
                free(pClient->players[i]);
            }
        }
        free(pClient);
    }
    SDLNet_Quit();
}

int receiveFromServer(Client *pClient, Player *player, UDPpacket *packet, UDPsocket udpSocket) {
    pClient->pPacket = SDLNet_AllocPacket(sizeof(PlayerPackage));
    pClient->udpSocket = SDLNet_UDP_Open(0);
    if (!pClient->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }

    if (!pClient->udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_FreePacket(packet);
        return 1;
    }
    if (SDLNet_UDP_Recv(pClient->udpSocket, pClient->pPacket) > 0) {
        PlayerPackage *pkg = (PlayerPackage *)pClient->pPacket->data;

        player->playerX = pkg->x;
        player->playerY = pkg->y;
        player->direction = pkg->direction;

        printf("Received player data: X=%d, Y=%d, Direction=%d\n", player->playerX, player->playerY, player->direction);
    } else {
        fprintf(stderr, "No data received from server\n");
    }

    printf("udpsocket: %p\n", pClient->udpSocket);
    printf("packet: %p\n", pClient->pPacket);

    SDLNet_FreePacket(pClient->pPacket);
    SDLNet_UDP_Close(pClient->udpSocket);

    return 1; 
}


void sendDataUDP(Client *pClient, Player *player, UDPpacket *packet, UDPsocket udpSocket) {
    if (!pClient || !player) {
        printf("Error: Null pointer\n");
        return;
    }

    pClient->pPacket = SDLNet_AllocPacket(sizeof(PlayerPackage));
    pClient->udpSocket = SDLNet_UDP_Open(0);
    if (!pClient->pPacket) {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        // handle error
    }

    PlayerPackage pkg;
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.direction = player->direction; 

    memcpy(pClient->pPacket->data, &pkg, sizeof(PlayerPackage));

    pClient->pPacket->len = sizeof(PlayerPackage);

    pClient->pPacket->address.host = pClient->ip.host;
    pClient->pPacket->address.port = UDP_PORT;

    printf("udpsocket=%p\n", pClient->udpSocket);
    printf("packet=%p\n", pClient->pPacket);
    SDLNet_UDP_Send(pClient->udpSocket, -1, pClient->pPacket);
}*/