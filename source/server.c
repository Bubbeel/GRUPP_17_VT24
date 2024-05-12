#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/server.h"
#include "../objects/client.h"
#include "../objects/player.h"
#include "../objects/common.h"


#define PORT 3000
#define UDPPORT 3001

Server *createServer();
int acceptClientConnections(Server *pServer, int *pNumClients, Client *client);
int sendMessageToClient(TCPsocket clientSocket);
void handlePlayerMovementData(char *data, Client *client, int *pNumClients, Server *pServer);
void listenForClientData(Server *pServer, Client *pClients, int *pNumClients);
void sendGameData(Server *pServer, Client *pClient, Player *player);
void sendPlayerPosition(Client *client, Player *player);

Server *createServer() {
    IPaddress ip;
    Server *pServer = malloc(sizeof(Server));
    int *pNumClients=malloc(sizeof(int));
    *pNumClients=0;
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

int acceptClientConnections(Server *pServer, int *pNumClients, Client *client) {
    TCPsocket clientSocket;
    GameObject gameObject;
    const char *message = "You are connected";

    while(1) {
        clientSocket = SDLNet_TCP_Accept(pServer->serverSocket);
        if (clientSocket) {
            printf("Client connected!\n");
            (*pNumClients)++;
            printf("Number of clients: %d\n", *pNumClients);
            if (sendMessageToClient(clientSocket) != 0) {
                SDLNet_TCP_Close(clientSocket);
                return 1;
            }
            
            // Allocate memory for the client
            Client *newClient = malloc(sizeof(Client));
            if (!newClient) {
                fprintf(stderr, "Error: Failed to allocate memory for client\n");
                SDLNet_TCP_Close(clientSocket);
                return 1;
            }
            
            newClient->player[*pNumClients] = client->player[*pNumClients];

            // Add the new client to the array of clients
            pServer->clients[*pNumClients] = *newClient;
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
void handlePlayerMovementData(char *data, Client *client, int *pNumClients, Server *pServer) {
    int playerX, playerY;
    sscanf(data, "%d,%d", &playerX, &playerY);

    client->player[client->clientId]->playerX = playerX;
    client->player[client->clientId]->playerY = playerY;
}

void sendPlayerPosition(Client *client, Player *player) {
    UDPpacket *packet = SDLNet_AllocPacket(512);
    if (!packet) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return;
    }
    PlayerPackage pkg;
    pkg.clientId = client->clientId;
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.direction = player->direction;

    memcpy(packet->data, &pkg, sizeof(PlayerPackage));
    packet->len = sizeof(PlayerPackage);

    packet->address = client->ip;
    packet->address.port = UDPPORT;
    SDLNet_UDP_Send(client->udpSocket, -1, packet);
}

void listenForClientData(Server *pServer, Client *pClients, int *pNumClients) {
    UDPsocket udpSocket = SDLNet_UDP_Open(3001);
    int channel = -1;
    if (!udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return;
    }
    UDPpacket *pPacket = SDLNet_AllocPacket(512);
    if (!pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(udpSocket);
        return;
    }
    if (SDLNet_UDP_Recv(udpSocket, pPacket) > 0) {
        IPaddress *clientAddress = &pPacket->address;

        // Find the index of the client that sent the data
        for (int i = 0; i < *pNumClients; i++) {
            if (pClients->ip.host == clientAddress->host && pClients->ip.port == clientAddress->port) {
                pClients->clientId = i;
                break;
            }
        }

        // If the client index is found, handle the received data
        if (pClients->clientId != -1) {
            // Extract the data from the packet
            char *data = (char *)pPacket->data;

            // Process the received data (not implemented)
            handlePlayerMovementData(data, &pClients[pClients->clientId], pNumClients, pServer);

            // After processing, send the updated player position to all other clients
            for (int i = 0; i < *pNumClients; i++) {
                if (pClients->player[i] == pClients->player[pClients->clientId]) {
                    sendPlayerPosition(pClients, pClients->player[pClients->clientId]);
                }
            }
        }
    }

    // Cleanup code (not reached in this loop)
    SDLNet_UDP_Close(udpSocket);
    SDLNet_FreePacket(pPacket);
}
