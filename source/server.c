#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/server.h"
#include "../objects/client.h"
#include "../objects/player.h"
#include "../objects/common.h"


#define PORT 3000
#define UDPPORT 3001

Server *createServer();
int acceptClientConnections(Server *pServer, int *pNrOfClients, Client *client);
int sendMessageToClient(TCPsocket clientSocket);
void handlePlayerMovementData(char *data, Client *client, int *pNrOfClients, Server *pServer);
void listenForClientData(Server *pServer, Client *pClients, int *pNrOfClients);
void sendGameData(Server *pServer, Client *pClient, Player *player);
void sendPlayerPosition(Client *client, Player *player);

Server *createServer() {
    IPaddress ip;
    Server *pServer = malloc(sizeof(Server));
    pServer->pNrOfClients = malloc(sizeof(int));
    *pServer->pNrOfClients = 0;
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

int acceptClientConnections(Server *pServer, int *pNrOfClients, Client *client) {
    TCPsocket clientSocket;
    GameObject gameObject;
    const char *message = "You are connected";
   

    while(1) {
        clientSocket = SDLNet_TCP_Accept(pServer->serverSocket);
        if (clientSocket) {
            printf("Client connected!\n");
            (*pServer->pNrOfClients)++;
            
            // Send confirmation message to client
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
            
            newClient->player[*pServer->pNrOfClients] = client->player[*pServer->pNrOfClients];

            // Add the new client to the array of clients
            pServer->clients[*pServer->pNrOfClients] = *newClient;
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
void handlePlayerMovementData(char *data, Client *client, int *pNrOfClients, Server *pServer) {
    int playerX, playerY;
    sscanf(data, "%d,%d", &playerX, &playerY);

    client->player[client->clientId]->playerX = playerX;
    client->player[client->clientId]->playerY = playerY;
}

void sendPlayerPosition(Client *client, Player *player) {
    // Allocate memory for the packet
    /*void sendDataUDP(Client *pClient, Player *player) {
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
}*/
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

void listenForClientData(Server *pServer, Client *pClients, int *pNrOfClients) {
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
        for (int i = 0; i < *pNrOfClients; i++) {
            if ((SDLNet_UDP_GetPeerAddress(pClients[i].udpSocket, channel)->host == clientAddress->host) &&
                (SDLNet_UDP_GetPeerAddress(pClients[i].udpSocket, channel)->port == clientAddress->port)) {
                pClients->clientId = i;
                break;
            }
        }

        // If the client index is found, handle the received data
        if (pClients->clientId != -1) {
            // Extract the data from the packet
            char *data = (char *)pPacket->data;

            // Process the received data (not implemented)
            handlePlayerMovementData(data, &pClients[pClients->clientId], pNrOfClients, pServer);

            // After processing, send the updated player position to all other clients
            for (int i = 0; i < *pNrOfClients; i++) {
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
