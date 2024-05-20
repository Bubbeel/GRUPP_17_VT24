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

// Define a structure to store player input state
typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
} PlayerInputState;

int main(int argc, char **argv) {
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
    SDL_Texture* gridTexture = loadGridMap(pRenderer);
    Flag* flag = createFlag(pRenderer);
    if (!flag) {
        printf("Failed to create flag, Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    bool closeWindow = false;
    PlayerInputState inputState = {false, false, false, false};
    Server pServer = {0};
    Client pClient = {0};
    Player* pPlayer = NULL;

    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        SDL_DestroyWindow(pWindow);
        if (!createServer(&pServer, pRenderer, map)) {
            printf("Failed to create server\n");
            SDL_Quit();
            return 1;
        }
    } else {
        if (!createClient(&pClient, pRenderer, map)) {
            printf("Failed to create client\n");
            SDL_DestroyRenderer(pRenderer);
            SDL_DestroyWindow(pWindow);
            SDL_Quit();
            return 1;
        }
        pPlayer = pClient.players[pClient.playerNr];
        if (!pPlayer) {
            printf("Failed to initialize player\n");
            SDL_DestroyRenderer(pRenderer);
            SDL_DestroyWindow(pWindow);
            SDL_Quit();
            return 1;
        }
    }

    while (!closeWindow) {
        if (argc > 1 && strcmp(argv[1], "server") == 0) {
            // Server-side logic
            if (SDLNet_UDP_Recv(pServer.udpSocket, pServer.pPacket) > 0) {
                IPaddress clientAddress = pServer.pPacket->address;
                bool clientExists = false;
                for (int i = 0; i < pServer.nrOfClients; i++) {
                    if (clientAddress.host == pServer.clients[i].host && clientAddress.port == pServer.clients[i].port) {
                        clientExists = true;
                        break;
                    }
                }
                if (!clientExists && pServer.nrOfClients < MAX_CLIENTS) {
                    pServer.clients[pServer.nrOfClients] = clientAddress;
                    handleClientConnection(&pServer, pServer.nrOfClients, pRenderer, map);
                    pServer.nrOfClients++;
                    printf("Client connected: %s:%d\n", SDLNet_ResolveIP(&clientAddress), clientAddress.port);
                } else {
                    printf("Client already connected or maximum number of clients reached\n");
                }
            }
            listenForClientData(&pServer, &inputState);
            sendPlayerPositions(&pServer);
        } else {
            // Client-side logic
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        closeWindow = true;
                        break;
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.scancode) {
                            case SDL_SCANCODE_W:
                                inputState.up = true;
                                break;
                            case SDL_SCANCODE_A:
                                inputState.left = true;
                                break;
                            case SDL_SCANCODE_S:
                                inputState.down = true;
                                break;
                            case SDL_SCANCODE_D:
                                inputState.right = true;
                                break;
                        }
                        break;
                    case SDL_KEYUP:
                        switch (event.key.keysym.scancode) {
                            case SDL_SCANCODE_W:
                                inputState.up = false;
                                break;
                            case SDL_SCANCODE_A:
                                inputState.left = false;
                                break;
                            case SDL_SCANCODE_S:
                                inputState.down = false;
                                break;
                            case SDL_SCANCODE_D:
                                inputState.right = false;
                                break;
                        }
                        break;
                }
            }

            // Update player position locally based on input state
            handlePlayerInput(pPlayer, inputState.up, inputState.down, inputState.left, inputState.right, LEVEL_WIDTH, LEVEL_HEIGHT);

            // Send updated position to the server
            sendDataUDP(&pClient);

            // Receive updated positions from the server
            receiveFromServer(&pClient, pRenderer);

            // Render the player and other entities
            SDL_RenderClear(pRenderer);
            if (pPlayer) {
                renderGridMapCentered(pRenderer, map, pPlayer, WINDOW_WIDTH, WINDOW_HEIGHT, LEVEL_WIDTH, LEVEL_HEIGHT);
            }
            flagAnimation(pRenderer, flag);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (pClient.players[i]) {
                    renderPlayer(pClient.players[i], pRenderer);
                }
            }
            SDL_RenderPresent(pRenderer);
            SDL_Delay(1000 / PLAYER_FRAME_RATE);
        }
    }

    if (pPlayer) {
        destroyPlayer(pPlayer);
    }
    destroyFlag(flag);
    destroyGridMap(map);
    SDL_DestroyTexture(flag->flagTexture);
    SDL_DestroyTexture(gridTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    SDL_Quit();
    return 0;
}
