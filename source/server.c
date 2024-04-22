#include <stdio.h>
#include <SDL2/SDL_net.h>

int main() {
    UDPsocket serverSocket;
    UDPpacket *packet;
    IPaddress serverIP;

    SDLNet_Init();

    serverSocket = SDLNet_UDP_Open(3000);

    packet = SDLNet_AllocPacket(512);

    SDLNet_UDP_Recv(serverSocket, packet);

    printf("Message from client: %s\n", (char *)packet->data);

    SDLNet_UDP_Send(serverSocket, -1, packet);

    SDLNet_FreePacket(packet);
    
    SDLNet_Quit();

    return 0;
}