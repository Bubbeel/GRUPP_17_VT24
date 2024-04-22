#include <stdio.h>
#include <SDL2/SDL_net.h>

int main() {
    UDPsocket clientSocket;
    UDPpacket *packet;
    IPaddress serverIP;

    SDLNet_Init();

    clientSocket = SDLNet_UDP_Open(0);

    packet = SDLNet_AllocPacket(512);

    SDLNet_ResolveHost(&serverIP, "127.0.0.1", 3000);

    packet->address = serverIP;

    strcpy((char *)packet->data, "Hello World");
    
    packet->len = strlen((char *)packet->data) + 1;

    SDLNet_UDP_Send(clientSocket, -1, packet);

    SDLNet_UDP_Recv(clientSocket, packet);

    printf("Svar från servern: %s\n", (char *)packet->data);

    SDLNet_FreePacket(packet);
    SDLNet_Quit();

    return 0;
}
