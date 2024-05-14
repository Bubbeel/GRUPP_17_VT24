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
    *pServer = (Server){0};
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
    pServer->udpSocket = SDLNet_UDP_Open(UDPPORT);
    if (!pServer->udpSocket) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 0;
    }

    pServer->pPacket = SDLNet_AllocPacket(512);
    if (!pServer->pPacket) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(pServer->udpSocket);
        SDLNet_Quit();
        return 0;
    }
     for(int i = 0; i < MAX_CLIENTS; i++){
        pServer->players[i] = malloc(sizeof(Player));
        if (pServer->players[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory for player %d\n", i);
            return 0;
        }
    }
    for(int i = 0; i < MAX_CLIENTS; i++){
        if (pServer->players[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory for player %d\n", i);
            return 0;
        }
        pServer->players[i] = createPlayer(renderer);
    }
    pServer->nrOfPlayers=0;
    pServer->nrOfClients=0;
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
            pClient->players[i]->direction = pkg->direction;
        }
        printf("Received player data from client %d: X=%d, Y=%d, Direction=%d\n", pkg->clientId, player->playerX, player->playerY, player->direction);
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
        pkg.clientId = pServer->clientId; // Set client ID accordingly
        pkg.x = pClient->players[i]->playerX;
        pkg.y = pClient->players[i]->playerY;
        pkg.direction = pClient->players[i]->direction;

        memcpy(pServer->pPacket->data, &pkg, sizeof(PlayerPackage));
        pServer->pPacket->len = sizeof(PlayerPackage);
    }

    if (SDLNet_UDP_Send(pServer->udpSocket, 0, pServer->pPacket) == 0) {
        fprintf(stderr, "SDLNet_UDP_Send playerposition: %s\n", SDLNet_GetError());
    }
}

void handleClientConnection(Server *pServer, int clientIndex, GameObject *gameObject){
    pServer->players[clientIndex] = createPlayer(gameObject->renderer);
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

int waitForClients(Server *pServer) {
    GameObject *gameObject;
    printf("Waiting for clients to connect...\n");

    // Accept connections from clients until the maximum number of clients is reached
    while (pServer->nrOfClients < MAX_CLIENTS) {
        // Allocate memory for the client's address
        IPaddress *clientAddress = malloc(sizeof(IPaddress));
        if (!clientAddress) {
            fprintf(stderr, "Failed to allocate memory for client address\n");
            break;
        }

        // Wait for an incoming connection
        if (SDLNet_UDP_Recv(pServer->udpSocket, pServer->pPacket) > 0) {
            // Extract the sender's address from the received packet
            *clientAddress = pServer->pPacket->address;

            // Add the client's address to the server's list of clients
            add(*clientAddress, pServer->clients, pServer);
            pServer->nrOfClients++;

            printf("Client connected: %s:%d\n", SDLNet_ResolveIP(clientAddress), clientAddress->port);

            // Handle the new client connection
            handleClientConnection(pServer, pServer->nrOfClients - 1, gameObject);
            return 1;
        }
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
Client *createClient() {
    GameObject gameObject;
    Client *pClient = malloc(sizeof(Client));
    Server *pServer;
    *pClient = (Client){0};
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
    pClient->pPacket->address.host = ip.host;
    pClient->pPacket->address.port = ip.port;
    for(int i = 0; i < pServer->nrOfClients; i++){
        pClient->players[i] = createPlayer(gameObject.renderer); // Assuming createPlayer allocates memory
        if (!pClient->players[i]) {
            printf("Failed to create player for client %d\n", i);
            // Handle error
        }
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
        free(pClient);
    }
}


int sendDataUDP(Client *pClient, Player *player, Server *pServer) {
    PlayerPackage pkg;
    for(int i=0; i<pServer->nrOfClients; i++){
        pkg.x =  pClient->players[i]->playerX;
        pkg.y =  pClient->players[i]->playerY;
        pkg.direction =  pClient->players[i]->direction;

        memcpy(pClient->pPacket->data, &pkg, sizeof(PlayerPackage));
        pClient->pPacket->len = sizeof(PlayerPackage);

        // Set packet destination here if necessary

        if (SDLNet_UDP_Send(pClient->udpSocket, 0, pClient->pPacket) == 0) {
            fprintf(stderr, "SDLNet_UDP_Send client data: %s\n", SDLNet_GetError());
            return 0; // Failed to send data
        }
    }

    return 1; // Data sent successfully
}

int receiveFromServer(Client *pClient, Player *player, Server *pServer) {
    if (SDLNet_UDP_Recv(pClient->udpSocket, pClient->pPacket) > 0) {
        PlayerPackage *pkg = (PlayerPackage *)pClient->pPacket->data;

        for(int i=0; i<pServer->nrOfClients; i++){
            pClient->players[i]->playerX = pkg->x;
            pClient->players[i]->playerY = pkg->y;
            pClient->players[i]->direction = pkg->direction;
        }
        printf("Received player data from server: X=%d, Y=%d, Direction=%d\n", player->playerX, player->playerY, player->direction);
    } else {
        fprintf(stderr, "No data received from server\n");
    }
    return 1; 
}

void updateGame(Server *pServer, Client *pClient, Player *player) {
    listenForClientData(pServer, player, pClient);
    receiveFromServer(pClient, player, pServer);
    sendDataUDP(pClient, player, pServer);
}