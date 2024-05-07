/*

#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/server.h"

#define PORT 3000

Server createServer();
int acceptClientConnections(Server *pServer);
int sendMessageToClient(TCPsocket clientSocket);
//void sendGameData();

Server createServer() {
    IPaddress ip;
    Server server;

    if (SDLNet_ResolveHost(&ip, NULL, PORT) == -1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        server.serverSocket=NULL;
        return server;
    }

    server.serverSocket = SDLNet_TCP_Open(&ip);
    if (!server.serverSocket) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return server;
    }

    return server;
}

int acceptClientConnections(Server *pServer) {
    TCPsocket clientSocket;
    const char *message = "You are connected";

    while (1) {
        clientSocket = SDLNet_TCP_Accept(pServer->serverSocket);
        if (clientSocket) {
            printf("Client connected!\n");

            // Send confirmation message to client
            if (sendMessageToClient(clientSocket) != 0) {
                SDLNet_TCP_Close(clientSocket);
                return 1;
            }

            // Close socket for client
            SDLNet_TCP_Close(clientSocket);
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

/*void sendGameData(){

}

*/