#include <stdio.h>
#include <SDL2/SDL_net.h>

#define SERVER_IP "127.0.0.1"
#define PORT 3000

int main() {
    IPaddress ip;
    TCPsocket clientSocket;
    char serverResponse[256];

    //initialize SDL_net
    SDLNet_Init();

    //resolve the host name and port number into an IPaddress type
    if (SDLNet_ResolveHost(&ip, SERVER_IP, PORT) == -1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    //create a tcp-socket for client
    clientSocket = SDLNet_TCP_Open(&ip);
    if (!clientSocket) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    //if connection to server succeded
    if (clientSocket) {
        SDLNet_TCP_Recv(clientSocket, serverResponse, sizeof(serverResponse));
        printf("Server says: %s\n", serverResponse);

        //close socket for client
        SDLNet_TCP_Close(clientSocket);
    } else {
        printf("Failed to connect to server!\n");
    }

    //quit SDL_net
    SDLNet_Quit();

    return 0;
}
