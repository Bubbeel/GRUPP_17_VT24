#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "../objects/client.h"

#define SERVER_IP "127.0.0.1"
#define PORT 3000

Client createClient();
int connectToServer(Client *pClient);
int receiveFromServer(Client *pClient);
void closeClient(Client *pClient);

Client createClient(){
    Client client;
    client.clientSocket=NULL;
    return client;
}

int connectToServer(Client *pClient){
    //initialize SDL_Net
    if(SDLNet_Init()<0){
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        return 1;
    }

    if(SDLNet_ResolveHost(&pClient->ip, SERVER_IP, PORT)==-1){
        fprintf(stderr, "SDLNet_ResloveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    pClient->clientSocket = SDLNet_TCP_Open(&pClient->ip);
    if(!pClient->clientSocket){
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }
    return 0;
}

int receiveFromServer(Client *pClient){
    char serverResponse[256];

    if(SDLNet_TCP_Recv(pClient->clientSocket, serverResponse,sizeof(serverResponse))<=0){
        fprintf(stderr, "SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    printf("Server says: %s\n", serverResponse);
    return 0;
}

void closeClient(Client *pClient){
    if(pClient->clientSocket){
        SDLNet_TCP_Close(pClient->clientSocket);
    }
    SDLNet_Quit();
}

//void sendData(GameObject *pGameObject);