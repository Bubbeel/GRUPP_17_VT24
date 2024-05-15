#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../objects/player.h"
#include "../objects/flag.h"
#include "../objects/gridmap.h"
#include "../objects/serverClient.h"
#include "../objects/collisionDetection.h"
#include "../objects/menu.h"
#include "../objects/common.h"

#define LEVEL_WIDTH 2816
#define LEVEL_HEIGHT 2100

#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

#define CLOSE_DISTANCE_THRESHOLD 10
#define FLAG_SPEED 2
#define PLAYER_SPEED 2
#define MAX_CLIENTS 4

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* pWindow = SDL_CreateWindow("CTF Gaming", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!pWindow) {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pRenderer) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    GridMap* map = createGridMap();
    loadMapFromFile("resources/map.txt", map);
    SDL_Texture* gridTexture;
    gridTexture = loadGridMap(pRenderer);
    Flag* flag = createFlag(pRenderer);
    Player *pPlayer = createPlayer(pRenderer, map->cells[5][5].cellRect.x, map->cells[5][5].cellRect.y);
    if(!flag)
    {
        printf("Failed to create flag, Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    // Set up variables
    bool closeWindow = false;
    PlayerMovementData *movementData = malloc(sizeof(PlayerMovementData));
    Server pServer = {0};
    Client pClient = {0};
    int numClients = 0;

    // Create server instance
    if (argc > 1 && strcmp(argv[1], "server") == 0)
    {
        SDL_DestroyWindow(pWindow);
        if (!createServer(&pServer, pRenderer, map))
        {
            printf("Failed to create server\n");
            SDL_Quit();
            return 1;
        }
    }
    else
    {
        if (!createClient(&pClient, pRenderer, map, numClients))
        {
            printf("Failed to create client\n");
            return 1;
        }
    }

    // Main loop
    while (!closeWindow)
    {
        // Event handling
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    closeWindow = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            movementData->up = true;
                            break;
                        case SDL_SCANCODE_A:
                            movementData->left= true;
                            break;
                        case SDL_SCANCODE_S:
                            movementData->down = true;
                            break;
                        case SDL_SCANCODE_D:
                            movementData->right = true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_UP:
                            movementData->up= false;
                            break;
                        case SDL_SCANCODE_LEFT:
                            movementData->left= false;
                            break;
                        case SDL_SCANCODE_DOWN:
                            movementData->down = false;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            movementData->right= false;
                            break;
                    }
                    break;
            }
        }

        // Server logic (if running as server)
        if (argc > 1 && strcmp(argv[1], "server") == 0)
        {
            // Check for incoming connection requests
            if (SDLNet_UDP_Recv(pServer.udpSocket, pServer.pPacket) > 0)
            {
                // Extract client address from the received packet
                IPaddress clientAddress = pServer.pPacket->address;

                // Check if the client is already connected
                bool clientExists = false;
                for (int i = 0; i < pServer.nrOfClients; i++)
                {
                    if (clientAddress.host == pServer.clients[i].host && clientAddress.port == pServer.clients[i].port)
                    {
                        clientExists = true;
                        break;
                    }
                }

                // If the client is not already connected and the maximum number of clients has not been reached, accept the connection
                if (!clientExists && pServer.nrOfClients < MAX_CLIENTS)
                {
                    pServer.clients[pServer.nrOfClients] = clientAddress;
                    pServer.nrOfClients++;

                    printf("Client connected: %s:%d\n", SDLNet_ResolveIP(&clientAddress), clientAddress.port);

                    // Handle client connection (create player, send initial data, etc.)
                    handleClientConnection(&pServer, pServer.nrOfClients - 1, pRenderer, map);
                }
                else
                {
                    printf("Client already connected or maximum number of clients reached\n");
                }
            }

            // Listen for client data and send player position updates
            listenForClientData(&pServer, pPlayer, &pClient);
            sendPlayerPosition(&pClient, movementData, &pServer);

            // Handle other server logic here (updating game state, processing player movements, sending updates to clients, etc.)
        }
        else // Client logic
        {
            // Handle client input and send data to server
            if (movementData->up || movementData->down || movementData->left || movementData->right) {
                sendDataUDP(&pClient, pPlayer, &pServer);
            }

            // Receive data from server
            receiveFromServer(&pClient, pPlayer, &pServer);

            // Handle other client logic here (rendering remote players, etc.)
        }

        // Update game state, process player movements, collision detection, etc.

        // Render game state
        SDL_RenderClear(pRenderer);
        renderGridMapCentered(pRenderer, map, pPlayer, WINDOW_WIDTH, WINDOW_HEIGHT, LEVEL_WIDTH, LEVEL_HEIGHT);
        flagAnimation(pRenderer, flag);
        renderPlayer(pPlayer, pRenderer);
        SDL_RenderPresent(pRenderer);

        // Delay for frame rate control
        SDL_Delay(1000 / PLAYER_FRAME_RATE);
    }

    // Clean up resources
    destroyPlayer(pPlayer);
    destroyFlag(flag);
    destroyGridMap(map);
    SDL_DestroyTexture(pPlayer->pPlayerTexture);
    SDL_DestroyTexture(flag->flagTexture);
    SDL_DestroyTexture(gridTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    SDL_Quit();
    return 0;
}
