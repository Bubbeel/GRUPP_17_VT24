#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/serverClient.h"
#include "../objects/client.h"
#include "../objects/player.h"
#include "../objects/common.h"
#include "../objects/gridmap.h"

#define PORT 3000
#define UDPPORT 3001
#define MAX_CLIENTS 4
#define SERVER_IP "127.0.0.1"
#define UDP_PORT 3001
#define PLAYER_SPEED 2
#define GRID_WIDTH 88
#define GRID_HEIGHT 66

Server *createServer(Server *pServer, SDL_Renderer *pRenderer, GridMap *map) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }
    if (TTF_Init() != 0) {
        printf("Error: %s\n", TTF_GetError());
        SDL_Quit();
        return NULL;
    }
    if (SDLNet_Init()) {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    pServer->udpSocket = SDLNet_UDP_Open(UDPPORT);
    if (!pServer->udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return NULL;
    }
    pServer->pPacket = SDLNet_AllocPacket(512);
    if (!pServer->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pServer->udpSocket);
        SDLNet_Quit();
        return NULL;
    }
    pServer->nrOfPlayers = 0;
    pServer->nrOfClients = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pServer->players[i] = NULL;
    }
    return pServer;
}

void closeServer(Server *pServer) {
    if (pServer) {
        if (pServer->udpSocket) {
            SDLNet_UDP_Close(pServer->udpSocket);
        }
        if (pServer->pPacket) {
            SDLNet_FreePacket(pServer->pPacket);
        }
        SDLNet_Quit();
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (pServer->players[i]) {
                destroyPlayer(pServer->players[i]);
            }
        }
        free(pServer);
    }
}

void listenForClientData(Server *pServer, PlayerMovementData *movementData) {
    if (pServer == NULL) {
        fprintf(stderr, "Error: Server is NULL\n");
        return;
    }
    if (SDLNet_UDP_Recv(pServer->udpSocket, pServer->pPacket) > 0) {
        memcpy(movementData, pServer->pPacket->data, sizeof(PlayerMovementData));
        int clientId = movementData->playerNumber;

        if (clientId >= 0 && clientId < MAX_CLIENTS) {
            switch (movementData->command) {
                case MOVE_UP:
                    pServer->players[clientId]->playerY -= PLAYER_SPEED;
                    break;
                case MOVE_DOWN:
                    pServer->players[clientId]->playerY += PLAYER_SPEED;
                    break;
                case MOVE_LEFT:
                    pServer->players[clientId]->playerX -= PLAYER_SPEED;
                    break;
                case MOVE_RIGHT:
                    pServer->players[clientId]->playerX += PLAYER_SPEED;
                    break;
                case STOP_MOVE:
                    break;
            }
            printf("Received player command from client %d: %d\n", clientId, movementData->command);
        } else {
            fprintf(stderr, "Invalid client ID: %d\n", clientId);
        }
    } else {
        fprintf(stderr, "No data received from client\n");
    }
}

void sendPlayerPositions(Server *pServer) {
    if (pServer == NULL) {
        printf("Server is NULL\n");
        return;
    }
    if (pServer->pPacket == NULL) {
        printf("Packet is NULL\n");
        return;
    }
    PlayerPackage pkg[MAX_CLIENTS];
    for (int i = 0; i < pServer->nrOfClients; i++) {
        printf("Sending player %d position: X=%d, Y=%d\n", i, pServer->players[i]->playerX, pServer->players[i]->playerY);
        pkg[i].x = pServer->players[i]->playerX;
        pkg[i].y = pServer->players[i]->playerY;
        memcpy(pServer->pPacket->data + i * sizeof(PlayerPackage), &pkg[i], sizeof(PlayerPackage));
    }
    pServer->pPacket->len = pServer->nrOfClients * sizeof(PlayerPackage);
    for (int i = 0; i < pServer->nrOfClients; i++) {
        pServer->pPacket->address = pServer->clients[i];
        if (SDLNet_UDP_Send(pServer->udpSocket, -1, pServer->pPacket) == 0) {
            fprintf(stderr, "SDLNet_UDP_Send player positions: %s\n", SDLNet_GetError());
        } else {
            printf("Player positions sent to client %d\n", i);
        }
    }
}

void handleClientConnection(Server *pServer, int clientIndex, SDL_Renderer *pRenderer, GridMap *map) {
    int x = 5;
    int y = 5;
    while (map->cells[x][y].type != EMPTY) {
        x = (x + 1) % GRID_WIDTH;
        y = (y + 1) % GRID_HEIGHT;
    }
    pServer->players[clientIndex] = createPlayer(pRenderer, map->cells[x][y].cellRect.x, map->cells[x][y].cellRect.y);
    if (!pServer->players[clientIndex]) {
        printf("Failed to create player for client %d\n", clientIndex);
    } else {
        pServer->players[clientIndex]->playerX = map->cells[x][y].cellRect.x;
        pServer->players[clientIndex]->playerY = map->cells[x][y].cellRect.y;
        pServer->players[clientIndex]->playerRect.x = pServer->players[clientIndex]->playerX - pServer->players[clientIndex]->camera.x;
        pServer->players[clientIndex]->playerRect.y = pServer->players[clientIndex]->playerY - pServer->players[clientIndex]->camera.y;
        printf("Player created for client %d at (%d, %d)\n", clientIndex, x, y);
        pServer->nrOfPlayers++;
    }
}

int waitForClients(Server *pServer, SDL_Renderer *pRenderer, GridMap *map) {
    printf("Waiting for clients to connect...\n");
    IPaddress *clientAddress = malloc(sizeof(IPaddress));
    if (!clientAddress) {
        fprintf(stderr, "Failed to allocate memory for client address\n");
        return 0;
    }
    if (SDLNet_UDP_Recv(pServer->udpSocket, pServer->pPacket) > 0) {
        *clientAddress = pServer->pPacket->address;
        add(*clientAddress, pServer->clients, pServer);
        printf("Client connected: %s:%d\n", SDLNet_ResolveIP(clientAddress), clientAddress->port);
        handleClientConnection(pServer, pServer->nrOfClients, pRenderer, map);
        free(clientAddress);
        return 1;
    }
    free(clientAddress);
    return 0;
}

void add(IPaddress address, IPaddress clients[], Server *pServer) {
    for (int i = 0; i < pServer->nrOfClients; i++) {
        if (address.host == clients[i].host && address.port == clients[i].port) {
            return;
        }
    }
    clients[pServer->nrOfClients] = address;
    pServer->nrOfClients++;
}

Client *createClient(Client *pClient, SDL_Renderer *pRenderer, GridMap *map) {
    if (!pClient) {
        fprintf(stderr, "Failed to allocate memory for client\n");
        return NULL;
    }

    pClient->nrOfPlayers = 0;
    pClient->playerNr = 0;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }

    if (TTF_Init() != 0) {
        printf("Error: %s\n", TTF_GetError());
        SDL_Quit();
        return NULL;
    }

    if (SDLNet_Init()) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    pClient->udpSocket = SDLNet_UDP_Open(0);
    if (!pClient->udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return NULL;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, SERVER_IP, UDP_PORT) == -1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_Quit();
        return NULL;
    }

    pClient->pPacket = SDLNet_AllocPacket(512);
    if (!pClient->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_Quit();
        return NULL;
    }

    pClient->pPacket->address.host = ip.host;
    pClient->pPacket->address.port = ip.port;

    // Create the player's initial position
    pClient->players[pClient->playerNr] = createPlayer(pRenderer, map->cells[5][5].cellRect.x, map->cells[5][5].cellRect.y);
    if (!pClient->players[pClient->playerNr]) {
        fprintf(stderr, "Failed to create player\n");
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_FreePacket(pClient->pPacket);
        SDLNet_Quit();
        return NULL;
    }

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
        SDLNet_Quit();
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (pClient->players[i]) {
                destroyPlayer(pClient->players[i]);
            }
        }
        free(pClient);
    }
}

int sendDataUDP(Client *pClient, PlayerMovementData *movementData) {
    memcpy(pClient->pPacket->data, movementData, sizeof(PlayerMovementData));
    pClient->pPacket->len = sizeof(PlayerMovementData);

    if (SDLNet_UDP_Send(pClient->udpSocket, -1, pClient->pPacket) == 0) {
        fprintf(stderr, "SDLNet_UDP_Send client data: %s\n", SDLNet_GetError());
        return 0; // Failed to send data
    }
    printf("Data sent to server\n");
    return 1; // Data sent successfully
}

int receiveFromServer(Client *pClient, SDL_Renderer *pRenderer) {
    if (SDLNet_UDP_Recv(pClient->udpSocket, pClient->pPacket) > 0) {
        PlayerPackage *pkg = (PlayerPackage *)pClient->pPacket->data;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (pClient->players[i] == NULL) {
                pClient->players[i] = createPlayer(pRenderer, pkg[i].x, pkg[i].y);
            } else {
                pClient->players[i]->playerX = pkg[i].x;
                pClient->players[i]->playerY = pkg[i].y;
                // Update playerRect after setting playerX and playerY
                pClient->players[i]->playerRect.x = pClient->players[i]->playerX - pClient->players[i]->camera.x;
                pClient->players[i]->playerRect.y = pClient->players[i]->playerY - pClient->players[i]->camera.y;
            }
        }
        printf("Received player data from server: X=%d, Y=%d\n", pClient->players[pClient->playerNr]->playerX, pClient->players[pClient->playerNr]->playerY);
    } else {
        fprintf(stderr, "No data received from server\n");
    }
    return 1;
}
