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
void listenForClientData(Server *pServer, Client *pClients, int *pNumClients, UDPsocket udpSocket, UDPpacket *packet);
void sendPlayerPosition(Server *pServer, Client *client, Player *player, UDPsocket udpSocket, UDPpacket *packet);


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
    if (pServer->serverSocket==NULL) {
        fprintf(stderr, "SDLNet_TCP_Open server: %s\n", SDLNet_GetError());
        return pServer;
    }
    pServer->udpSocket = SDLNet_UDP_Open(UDPPORT);
    pServer->pPacket = SDLNet_AllocPacket(512);
    printf("serverSocket: %p\n", pServer->serverSocket);
    if (pServer->serverSocket==NULL) {
        fprintf(stderr, "SDLNet_TCP_Open server: %s\n", SDLNet_GetError());
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
    printf("client id: %d", client->clientId);
    client->player[client->clientId]->playerX = playerX;
    client->player[client->clientId]->playerY = playerY;
}

void sendPlayerPosition(Server *server, Client *client, Player *player, UDPsocket udpSocket, UDPpacket *packet) {
    // Check if client and player pointers are valid
    if (!client || !player) {
        printf("Error: Null pointer\n");
        return;
    }

    // Create a new packet to send player position data
    if (!server->pPacket) {
        printf("Error: Failed to allocate memory for packet\n");
        return;
    }

    // Populate the PlayerPackage struct with player position data
    PlayerPackage pkg;
    pkg.clientId = client->clientId;
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.direction = player->direction;

    // Copy the PlayerPackage struct into packet data
    memcpy(server->pPacket->data, &pkg, sizeof(PlayerPackage));

    // Set the length of the packet
    server->pPacket->len = sizeof(PlayerPackage);

    // Set the destination address of the packet
    server->pPacket->address.host = client->ip.host;
    server->pPacket->address.port = UDPPORT;

    // Send the packet over UDP
    SDLNet_UDP_Send(server->udpSocket, -1, server->pPacket);

    // Free the packet memory
    SDLNet_FreePacket(server->pPacket);
}

void listenForClientData(Server *pServer, Client *pClients, int *pNumClients, UDPsocket udpSocket, UDPpacket *packet) {
    if (!pServer->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return;
    }
    if (SDLNet_UDP_Recv(pServer->udpSocket, pServer->pPacket) > 0) {
        IPaddress *clientAddress = &pServer->pPacket->address;

        // Find the index of the client that sent the data
        int clientIndex = -1;
        for (int i = 0; i < *pNumClients; i++) {
            if (pClients[i].ip.host == clientAddress->host && pClients[i].ip.port == clientAddress->port) {
                clientIndex = i;
                break;
            }
        }
        // If the client index is found, handle the received data
        if (clientIndex != -1) {
            // Extract the data from the packet
            char *data = (char *)pServer->pPacket->data;

            // Process the received data (not implemented)
            handlePlayerMovementData(data, &pClients[clientIndex], pNumClients, pServer);

            // After processing, send the updated player position to all other clients
            for (int i = 0; i < *pNumClients; i++) {
                if (pClients->player[i] == pClients->player[clientIndex]) {
                    sendPlayerPosition(pServer, &pClients[i], pClients[clientIndex].player[clientIndex], pServer->udpSocket, pServer->pPacket);
                }
            }
        }
    }else{
        printf("No data received from client\n");
    }

    // Cleanup code (not reached in this loop)
    SDLNet_FreePacket(pServer->pPacket);
}
