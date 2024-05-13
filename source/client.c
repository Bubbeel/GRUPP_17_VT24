/*#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/client.h"
#include "../objects/player.h"
#include "../objects/common.h"

#define SERVER_IP "127.0.0.1"
#define UDP_PORT 3001

Client *createClient() {
    Client *pClient = malloc(sizeof(Client));
    pClient->clientId = -1;
    if (!pClient) {
        fprintf(stderr, "Error: Failed to allocate memory for client\n");
        return NULL;
    }

    if (SDLNet_Init() < 0) {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        free(pClient);
        return NULL;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, SERVER_IP, UDP_PORT) == -1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        free(pClient);
        return NULL;
    }

    pClient->udpSocket = SDLNet_UDP_Open(0);
    if (!pClient->udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        free(pClient);
        return NULL;
    }

    pClient->pPacket = SDLNet_AllocPacket(512);
    if (!pClient->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_Quit();
        free(pClient);
        return NULL;
    }

    pClient->set = SDLNet_AllocSocketSet(1);
    if (!pClient->set) {
        fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        SDLNet_FreePacket(pClient->pPacket);
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_Quit();
        free(pClient);
        return NULL;
    }
    if (SDLNet_UDP_Bind(pClient->udpSocket, -1, &ip) == -1) {
        fprintf(stderr, "SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
        SDLNet_FreePacket(pClient->pPacket);
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_Quit();
        free(pClient);
        return NULL;
    }

    SDLNet_UDP_AddSocket(pClient->set, pClient->udpSocket);

    return pClient;
}

void closeClient(Client *pClient) {
    if (pClient) {
        if (pClient->udpSocket) {
            SDLNet_UDP_Close(pClient->udpSocket);
        }
        if (pClient->pPacket) {
            SDLNet_FreePacket(pClient->pPacket);
        }
        if (pClient->set) {
            SDLNet_FreeSocketSet(pClient->set);
        }
        SDLNet_Quit();
        free(pClient);
    }
}

int sendDataUDP(Client *pClient, Player *player) {
    PlayerPackage pkg;
    pkg.clientId = pClient->clientId;
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.direction = player->direction;

    memcpy(pClient->pPacket->data, &pkg, sizeof(PlayerPackage));
    pClient->pPacket->len = sizeof(PlayerPackage);

    // Set packet destination here if necessary

    if (SDLNet_UDP_Send(pClient->udpSocket, -1, pClient->pPacket) == 0) {
        fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
        return 0; // Failed to send data
    }

    return 1; // Data sent successfully
}

int receiveFromServer(Client *pClient, Player *player) {
    if (SDLNet_CheckSockets(pClient->set, 0) > 0 && SDLNet_SocketReady(pClient->udpSocket)) {
        if (SDLNet_UDP_Recv(pClient->udpSocket, pClient->pPacket) > 0) {
            PlayerPackage *pkg = (PlayerPackage *)pClient->pPacket->data;

            player->playerX = pkg->x;
            player->playerY = pkg->y;
            player->direction = pkg->direction;

            printf("Received player data: X=%d, Y=%d, Direction=%d\n", player->playerX, player->playerY, player->direction);
        } else {
            fprintf(stderr, "No data received from server\n");
        }
    }

    return 1; 
}
*/