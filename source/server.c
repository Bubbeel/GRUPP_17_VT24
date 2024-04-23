#include <stdio.h>
#include <SDL2/SDL_net.h>

#define PORT 3000

int main() {
    TCPsocket serverSocket;
    IPaddress ip;
    TCPsocket clientSocket;

    //initialize sdl_net
    SDLNet_Init();  

    //resolve the host name and port number into an IPaddress type
    if (SDLNet_ResolveHost(&ip, NULL, PORT) == -1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    //create a tcp-socket for server
    serverSocket = SDLNet_TCP_Open(&ip);
    if (!serverSocket) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    //listen on incoming connections from client
    while (1) {
        clientSocket = SDLNet_TCP_Accept(serverSocket);
        if (clientSocket) {
            printf("Client connected!\n");

            //confirmation message to client
            const char* message = "You are connected";
            SDLNet_TCP_Send(clientSocket, message, strlen(message)+1);

            //close socket for client
            SDLNet_TCP_Close(clientSocket);
        }
    }

    //close serversocket and quit sdlnet
    SDLNet_TCP_Close(serverSocket);
    SDLNet_Quit();

    return 0;
}
