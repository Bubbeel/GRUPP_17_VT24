#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/serverClient.h"
#include "../objects/client.h"
#include "../objects/player.h"
#include "../objects/common.h"


#define PORT 3000
#define UDPPORT 3001
#define MAX_CLIENTS 4
#define SERVER_IP "127.0.0.1"
#define UDP_PORT 3001


Server *createServer() {
    Server *pServer = malloc(sizeof(Server));
    if (!pServer) {
        fprintf(stderr, "Error: Failed to allocate memory for server\n");
        return NULL;
    }
    pServer->clients = malloc(4*sizeof(Client));

    if (SDLNet_Init() < 0) {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        free(pServer);
        return NULL;
    }

    pServer->udpSocket = SDLNet_UDP_Open(UDPPORT);
    if (!pServer->udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        free(pServer);
        return NULL;
    }

    pServer->pPacket = SDLNet_AllocPacket(512);
    if (!pServer->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pServer->udpSocket);
        SDLNet_Quit();
        free(pServer);
        return NULL;
    }

    pServer->set = SDLNet_AllocSocketSet(4);
    if (!pServer->set) {
        fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        SDLNet_FreePacket(pServer->pPacket);
        SDLNet_UDP_Close(pServer->udpSocket);
        SDLNet_Quit();
        free(pServer);
        return NULL;
    }

    if (SDLNet_UDP_AddSocket(pServer->set, pServer->udpSocket) == -1) {
        fprintf(stderr, "SDLNet_UDP_AddSocket: %s\n", SDLNet_GetError());
        SDLNet_FreePacket(pServer->pPacket);
        SDLNet_UDP_Close(pServer->udpSocket);
        SDLNet_Quit();
        free(pServer);
        return NULL;
    }

    return pServer;
}

int acceptClientConnections(Server *pServer, int *pNumClients, Client *pClient) {
    UDPpacket *pPacket = SDLNet_AllocPacket(512);
    while(1){
        if (SDLNet_UDP_Recv(pServer->udpSocket, pPacket) > 0) {
            // Add the newly connected client to the list of clients
            if (*pNumClients < MAX_CLIENTS) {
                // Allocate memory for the new client
                pServer->clients[*pNumClients] = pServer->clients[*pNumClients];
                pServer->clients[*pNumClients].udpSocket = pServer->udpSocket;
                pServer->clients[*pNumClients].pPacket = pPacket;
                (*pNumClients)++;
                return 0; // Success
            } else {
                // If the maximum number of clients has been reached, reject the connection
                fprintf(stderr, "Maximum number of clients reached\n");
                return -1; // Error
            }
        } else {
            fprintf(stderr, "Failed to receive data from client\n");
            return -1; // Error
        }
    }
}

void closeServer(Server *pServer) {
    if (pServer) {
        if (pServer->udpSocket) {
            SDLNet_UDP_Close(pServer->udpSocket);
        }
        if (pServer->pPacket) {
            SDLNet_FreePacket(pServer->pPacket);
        }
        if (pServer->set) {
            SDLNet_FreeSocketSet(pServer->set);
        }
        SDLNet_Quit();
        free(pServer);
        free(pServer->clients);
    }
}

int listenForClientData(Server *pServer, Player *player) {
    if (pServer == NULL) {
        fprintf(stderr, "Error: Server is NULL\n");
        return -1;
    }
    if (SDLNet_CheckSockets(pServer->set, 0) > 0 && SDLNet_SocketReady(pServer->udpSocket)) {
        if (SDLNet_UDP_Recv(pServer->udpSocket, pServer->pPacket) > 0) {
            PlayerPackage *pkg = (PlayerPackage *)pServer->pPacket->data;

            // Update player position or state based on received data
            player->playerX = pkg->x;
            player->playerY = pkg->y;
            player->direction = pkg->direction;

            printf("Received player data from client %d: X=%d, Y=%d, Direction=%d\n", pkg->clientId, player->playerX, player->playerY, player->direction);
        } else {
            fprintf(stderr, "No data received from client\n");
        }
    }

    return 1; 
}

void sendPlayerPosition(Client *pClient, PlayerMovementData *movementData, Server *pServer) {
    Player *player = pClient->players[pClient->clientId];
     if (pServer == NULL) {
        printf("Server is NULL\n");
    }
    if(pServer->pPacket == NULL){
        printf("packet is NULL\n");
    }
    PlayerPackage pkg;
    pkg.clientId = pClient->clientId; // Set client ID accordingly
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.direction = player->direction;

    memcpy(pServer->pPacket->data, &pkg, sizeof(PlayerPackage));
    pServer->pPacket->len = sizeof(PlayerPackage);

    // Set packet destination here if necessary

    if (SDLNet_UDP_Send(pServer->udpSocket, 0, pServer->pPacket) == 0) {
        fprintf(stderr, "SDLNet_UDP_Send playerposition: %s\n", SDLNet_GetError());
    }
}

Client *createClient() {
    GameObject gameObject;
    Client *pClient = malloc(sizeof(Client));
    pClient->clientId = 0;
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
     for (int i = 0; i < MAX_CLIENTS; i++) {
        pClient->players[i] = createPlayer(gameObject.renderer); // Assuming createPlayer allocates memory
        if (!pClient->players[i]) {
            printf("Failed to create player for client %d\n", i);
            // Handle error
        }
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

    if (SDLNet_UDP_Send(pClient->udpSocket, 0, pClient->pPacket) == 0) {
        fprintf(stderr, "SDLNet_UDP_Send client data: %s\n", SDLNet_GetError());
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

void updateGame(Server *pServer, Client *pClient, Player *player) {
    listenForClientData(pServer, player);
    receiveFromServer(pClient, player);
    sendDataUDP(pClient, player);
}