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


Server *createServer(Server *pServer, SDL_Renderer *pRenderer, GridMap *map) {
    SDL_Renderer *renderer;
     if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 0;
    }
    if(TTF_Init()!=0){
        printf("Error: %s\n",TTF_GetError());
        SDL_Quit();
        return 0;
    }
    if (SDLNet_Init()){
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        TTF_Quit();
        SDL_Quit();
		return 0;
	}
    //pServer->udpSocket = SDLNet_UDP_Open(UDPPORT);
    if (!(pServer->udpSocket = SDLNet_UDP_Open(UDPPORT))) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 0;
    }

    //pServer->pPacket = SDLNet_AllocPacket(512);
    if (!(pServer->pPacket= SDLNet_AllocPacket(512))) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pServer->udpSocket);
        SDLNet_Quit();
        return 0;
    }
    pServer->nrOfPlayers=4;
    pServer->nrOfClients=0;
    for(int i = 0; i < MAX_CLIENTS; i++){
        printf("Creating player %d\n", i);
        pServer->players[i] = createPlayer(pRenderer, map->cells[5][5].cellRect.x, map->cells[5][5].cellRect.y);
        if (pServer->players[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory for player %d\n", i);
            return 0;
        }
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
        for(int i = 0; i < MAX_CLIENTS; i++)
        {
            free(pServer->players[i]);
        }
        free(pServer);
    }
}

int listenForClientData(Server *pServer, Player *player, Client *pClient) {
    if (pServer == NULL) {
        fprintf(stderr, "Error: Server is NULL\n");
        return -1;
    }
    // Check for incoming UDP packets
    if (SDLNet_UDP_Recv(pServer->udpSocket, pServer->pPacket) > 0) {
        // Extract the data from the received packet
        PlayerPackage *pkg = (PlayerPackage *)pServer->pPacket->data;
        for(int i=0; i<pServer->nrOfClients; i++){
            pClient->players[i]->playerX = pkg->x;
            pClient->players[i]->playerY = pkg->y;
        }
        printf("Received player data from client %d: X=%d, Y=%d\n", pkg->clientId, player->playerX, player->playerY);
    } else {
        fprintf(stderr, "No data received from client\n");
    }

    return 1; 
}

void sendPlayerPosition(Client *pClient, PlayerMovementData *movementData, Server *pServer) {
    if (pServer == NULL) {
        printf("Server is NULL\n");
    }
    if(pServer->pPacket == NULL){
        printf("packet is NULL\n");
    }
    PlayerPackage pkg;
    for(int i=0; i<pServer->nrOfClients; i++){
        printf("nrofclients %d\n", pServer->nrOfClients);
        pkg.clientId = pServer->clientId;
        pkg.x = pClient->players[i]->playerX;
        pkg.y = pClient->players[i]->playerY;

        memcpy(pServer->pPacket->data, &pkg, sizeof(PlayerPackage));
        pServer->pPacket->len = sizeof(PlayerPackage);
    }

    if (SDLNet_UDP_Send(pServer->udpSocket, 0, pServer->pPacket) == 0) {
        fprintf(stderr, "SDLNet_UDP_Send playerposition: %s\n", SDLNet_GetError());
    }
}

void handleClientConnection(Server *pServer, int clientIndex, SDL_Renderer *pRenderer, GridMap *map){
    pServer->players[clientIndex] = createPlayer(pRenderer, map->cells[5][5].cellRect.x, map->cells[5][5].cellRect.y);
    if (!pServer->players[clientIndex]) {
        printf("Failed to create player for client %d\n", clientIndex);
        // Handle error
    } else {
        pServer->nrOfPlayers++;
    }

    // Send any initial data or messages to the client if needed
    // Example: Send a welcome message
    // sendMessageToClient(pServer, clientIndex, "Welcome to the server!");
}

int waitForClients(Server *pServer, int clientIndex, SDL_Renderer *pRenderer, GridMap *map) {
    printf("Waiting for clients to connect...\n");

    while (pServer->nrOfClients < MAX_CLIENTS) {
        IPaddress *clientAddress = malloc(sizeof(IPaddress));
        if (!clientAddress) {
            fprintf(stderr, "Failed to allocate memory for client address\n");
            break;
        }

        if (SDLNet_UDP_Recv(pServer->udpSocket, pServer->pPacket) > 0) {
            *clientAddress = pServer->pPacket->address;
            add(*clientAddress, pServer->clients, pServer);
            pServer->nrOfClients++;

            printf("Client connected: %s:%d\n", SDLNet_ResolveIP(clientAddress), clientAddress->port);

            handleClientConnection(pServer, clientIndex, pRenderer, map);
            free(clientAddress); // Free allocated memory for client address
            return 1;
        }
        free(clientAddress); // Free allocated memory for client address if no connection
    }
    return 0;
}

void add(IPaddress address, IPaddress clients[], Server *pServer){
    for(int i=0; i<pServer->nrOfClients; i++){
        if(address.host==clients[i].host && address.port==clients[i].port){
            return;
        }
    }
    clients[pServer->nrOfClients]=address;
}


//////////////////////////////////////////////CLIENT//////////////////////////////////////////////////////////
Client *createClient(Client *pClient,SDL_Renderer *pRenderer, GridMap *map, int nrOfClients) {
    pClient->nrOfPlayers=0;
    pClient->playerNr=0;
    
    if (!pClient) {
        fprintf(stderr, "Failed to allocate memory for client\n");
        return NULL;
    }
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 0;
    }

    if(TTF_Init()!=0){
        printf("Error: %s\n",TTF_GetError());
        SDL_Quit();
        return 0;
    }
    if (SDLNet_Init())
	{
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
        TTF_Quit();
        SDL_Quit();
		return 0;
	}

    pClient->udpSocket = SDLNet_UDP_Open(0);
    if (!pClient->udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 0;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, SERVER_IP, UDP_PORT) == -1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 0;
    }

    pClient->pPacket = SDLNet_AllocPacket(512);
    if (!pClient->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pClient->udpSocket);
        SDLNet_Quit();
        return 0;
    }  
    for(int i = 0; i < nrOfClients; i++){
        pClient->players[i] = createPlayer(pRenderer, map->cells[5][5].cellRect.x, map->cells[5][5].cellRect.y); // Assuming createPlayer allocates memory
        if (!pClient->players[i]) {
            printf("Failed to create player for client %d\n", i);
        }
    }
    pClient->pPacket->address.host = ip.host;
    pClient->pPacket->address.port = ip.port;
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
        for(int i = 0; i < MAX_CLIENTS; i++)
        {
            free(pClient->players[i]);
        }
        free(pClient);
    }
}


int sendDataUDP(Client *pClient, Player *player, Server *pServer) {
    PlayerPackage pkg;
    pkg.x = player->playerX;
    pkg.y = player->playerY;
    pkg.clientId = pClient->playerNr;

    memcpy(pClient->pPacket->data, &pkg, sizeof(PlayerPackage));
    pClient->pPacket->len = sizeof(PlayerPackage);

    if (SDLNet_UDP_Send(pClient->udpSocket, 0, pClient->pPacket) == 0) {
        fprintf(stderr, "SDLNet_UDP_Send client data: %s\n", SDLNet_GetError());
        return 0; // Failed to send data
    }
    return 1; // Data sent successfully
}

int receiveFromServer(Client *pClient, Player *player, Server *pServer) {
    if (SDLNet_UDP_Recv(pClient->udpSocket, pClient->pPacket) > 0) {
        PlayerPackage *pkg = (PlayerPackage *)pClient->pPacket->data;

        for(int i=0; i<pServer->nrOfClients; i++){
            pClient->players[i]->playerX = pkg->x;
            pClient->players[i]->playerY = pkg->y;
        }
        printf("Received player data from server: X=%d, Y=%d\n", player->playerX, player->playerY);
    } else {
        fprintf(stderr, "No data received from server\n");
    }
    return 1; 
}
