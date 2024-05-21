#include <stdio.h>
#include <stdlib.h> // Added for malloc and free
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
#define PLAYER_SPEED 10
#define GRID_WIDTH 88
#define GRID_HEIGHT 66
void printPlayerPositions(Server *pServer);


Server *createServer(Server *pServer, SDL_Renderer *pRenderer, GridMap *map) {
    // Initialize SDL and SDL_net
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

    // Open UDP socket
    pServer->udpSocket = SDLNet_UDP_Open(UDPPORT);
    if (!pServer->udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return NULL;
    }

    // Allocate packet for UDP communication
    pServer->pPacket = SDLNet_AllocPacket(512);
    if (!pServer->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pServer->udpSocket);
        SDLNet_Quit();
        return NULL;
    }

    // Initialize server data
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

// Server-Side Changes

void listenForClientData(Server *pServer) {
    if (pServer == NULL) {
        fprintf(stderr, "Error: Server is NULL\n");
        return;
    }
    while(SDLNet_UDP_Recv(pServer->udpSocket, pServer->pPacket) > 0) {
        PlayerPackage pkg;
        memcpy(&pkg, pServer->pPacket->data, sizeof(PlayerPackage));
        int clientId = pkg.playerNumber;

        if (clientId >= 0 && clientId < MAX_CLIENTS && pServer->players[clientId] != NULL) {
            Player *player = pServer->players[clientId];
            player->playerX = pkg.x;
            player->playerY = pkg.y;
            player->playerVelocityX = pkg.velocityX;
            player->playerVelocityY = pkg.velocityY;
            player->camera.x = pkg.cameraX;
            player->camera.y = pkg.cameraY;
            player->playerRect.w = pkg.width;
            player->playerRect.h = pkg.height;
            printf("Received player command from client %d: X=%d, Y=%d, velocityX=%d, velocityY=%d\n",
                   clientId, player->playerX, player->playerY, player->playerVelocityX, player->playerVelocityY);
        } else {
            fprintf(stderr, "Invalid client ID: %d or player does not exist\n", clientId);
        }
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

    // Ensure the packet can hold data for all players
    size_t packetSize = MAX_CLIENTS * sizeof(PlayerPackage);
    if (pServer->pPacket->maxlen < packetSize) {
        fprintf(stderr, "Packet buffer too small\n");
        return;
    }

    // Create the package containing all player positions
    PlayerPackage pkg[MAX_CLIENTS] = {0}; // Initialize array to zero
    for (int j = 0; j < MAX_CLIENTS; j++) {
        if (pServer->players[j] != NULL) {
            Player *player = pServer->players[j];
            pkg[j].playerNumber = j;
            pkg[j].x = player->playerX;
            pkg[j].y = player->playerY;
            pkg[j].velocityX = player->playerVelocityX;
            pkg[j].velocityY = player->playerVelocityY;
            pkg[j].width = player->playerRect.w;
            pkg[j].height = player->playerRect.h;
            pkg[j].cameraX = player->camera.x;
            pkg[j].cameraY = player->camera.y;
        } else {
            pkg[j].playerNumber = -1; // Indicate no player
        }
    }

    // Copy the package to the packet's data buffer
    memcpy(pServer->pPacket->data, pkg, packetSize);
    pServer->pPacket->len = packetSize;

    // Send the package to all connected clients
    for (int i = 0; i < pServer->nrOfClients; i++) {
        pServer->pPacket->address = pServer->clients[i];
        if (SDLNet_UDP_Send(pServer->udpSocket, -1, pServer->pPacket) == 0) {
            fprintf(stderr, "SDLNet_UDP_Send player positions: %s\n", SDLNet_GetError());
        } else {
            printf("Player positions sent to client %d\n", i);
        }
    }

    // Print current player positions
    printPlayerPositions(pServer);
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

    // Initialize all players to NULL
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pClient->players[i] = NULL;
    }

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

int sendDataUDP(Client *pClient) {
    if (!pClient) {
        fprintf(stderr, "Client structure is NULL\n");
        return 0;
    }
    if (!pClient->pPacket) {
        fprintf(stderr, "Packet is NULL\n");
        return 0;
    }

    Player *player = pClient->players[pClient->playerNr];
    if (!player) {
        fprintf(stderr, "Player is NULL\n");
        return 0;
    }

    PlayerPackage pkg;
    pkg.playerNumber = pClient->playerNr;
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.velocityX = player->playerVelocityX;
    pkg.velocityY = player->playerVelocityY;
    pkg.width = player->playerRect.w;
    pkg.height = player->playerRect.h;
    pkg.cameraX = player->camera.x;
    pkg.cameraY = player->camera.y;

    memcpy(pClient->pPacket->data, &pkg, sizeof(PlayerPackage));
    pClient->pPacket->len = sizeof(PlayerPackage);

    if (SDLNet_UDP_Send(pClient->udpSocket, -1, pClient->pPacket) == 0) {
        fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
        return 0;
    }

    return 1;
}

// Client-Side Changes

void debugPlayerPackage(const PlayerPackage *pkg) {
    printf("PlayerNumber=%d, X=%d, Y=%d, VelocityX=%d, VelocityY=%d, Width=%d, Height=%d, CameraX=%d, CameraY=%d\n",
           pkg->playerNumber, pkg->x, pkg->y, pkg->velocityX, pkg->velocityY, pkg->width, pkg->height, pkg->cameraX, pkg->cameraY);
}

int receiveFromServer(Client *pClient, SDL_Renderer *pRenderer) {
    if (SDLNet_UDP_Recv(pClient->udpSocket, pClient->pPacket) > 0) {
        if (pClient->pPacket->len < MAX_CLIENTS * sizeof(PlayerPackage)) {
            fprintf(stderr, "Packet size mismatch\n");
            return 0;
        }

        PlayerPackage *pkg = (PlayerPackage *)pClient->pPacket->data;

        printf("Received player data from server:\n");

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (pkg[i].playerNumber != -1) { // Ensure valid player data
                if (pClient->players[i] == NULL) {
                    // Create new player if not exists
                    pClient->players[i] = createPlayer(pRenderer, pkg[i].x, pkg[i].y);
                    if (!pClient->players[i]) {
                        fprintf(stderr, "Failed to create player\n");
                        return 0; // Return error if player creation fails
                    }
                    printf("Player created at X=%d, Y=%d\n", pkg[i].x, pkg[i].y);
                }
                // Update player attributes
                pClient->players[i]->playerX = pkg[i].x;
                pClient->players[i]->playerY = pkg[i].y;
                pClient->players[i]->playerVelocityX = pkg[i].velocityX;
                pClient->players[i]->playerVelocityY = pkg[i].velocityY;
                pClient->players[i]->playerRect.w = pkg[i].width;
                pClient->players[i]->playerRect.h = pkg[i].height;
                pClient->players[i]->camera.x = pkg[i].cameraX;
                pClient->players[i]->camera.y = pkg[i].cameraY;

                printf("Player %d: X=%d, Y=%d, velocityX=%d, velocityY=%d\n",
                       pkg[i].playerNumber, pkg[i].x, pkg[i].y, pkg[i].velocityX, pkg[i].velocityY);
            } else {
                // Destroy player if not exists
                if (pClient->players[i]) {
                    destroyPlayer(pClient->players[i]);
                    pClient->players[i] = NULL;
                }
            }
        }
        printf("\n");
    } else {
        fprintf(stderr, "No data received from server\n");
    }
    return 1;
}



void printPlayerPositions(Server *pServer) {
    printf("Current player positions:\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (pServer->players[i] != NULL) {
            Player *player = pServer->players[i];
            printf("Player %d (ID=%d): X=%d, Y=%d, velocityX=%d, velocityY=%d\n",
                   i, i, player->playerX, player->playerY, player->playerVelocityX, player->playerVelocityY);
        }
    }
    printf("\n");
}

