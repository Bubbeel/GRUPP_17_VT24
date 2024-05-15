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
    PlayerMovementData *movementData=malloc(sizeof(PlayerMovementData));
    Server pServer={0};
    Client pClient={0};
    int *numClients = malloc(sizeof(int));
    *numClients = 0;
    if (!pPlayer->playerSurface || !flag->flagSurface) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }
    
    SDL_Rect playerRect1;

    // playerRect2.w /= 20;
    // playerRect2.h /= 20;

    //Start positions
    // int player1X = playerRect1.w;
    // int player1Y = playerRect1.h; 

    flag->flagRect.x = map->cells[20][20].cellRect.x;
    flag->flagRect.y = map->cells[20][20].cellRect.y;
    flag->flagX = flag->flagRect.x;
    flag->flagX = flag->flagRect.y;

    movementData->up = false;  
    movementData->down = false;
    movementData->left = false;
    movementData->right = false;

    if (argc > 1 && strcmp(argv[1], "server") == 0)
    {
        // Create server instance
        if (!createServer(&pServer,pRenderer, map))
        {
            printf("Failed to create server\n");
            SDL_Quit();
            return 1;
        }

        // Wait for clients to connect
        while (*numClients < MAX_CLIENTS)
        {
            int clientIndex = waitForClients(&pServer, *numClients, pRenderer, map);
            if (clientIndex < 0)
            {
                printf("Failed to accept client.\n");
                continue;
            }

            (*numClients)++;
        }
        while(!closeWindow){
            sendPlayerPosition(&pClient, movementData, &pServer);
            listenForClientData(&pServer, pPlayer, &pClient);
        }
    }
    else
    {
        if (!createClient(&pClient, pRenderer, map, *numClients))
        {
            printf("Failed to create client sdoifoisfo\n");
            return 1;
        }
        
        while (!closeWindow)
        {
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
                if (movementData->up || movementData->down || movementData->left || movementData->right) {
                    sendDataUDP(&pClient, pPlayer, &pServer);
                }
            }
        }

    SDL_RenderClear(pRenderer);
    if (checkCollision(pPlayer, flag->flagRect))
    {
        flag->flagRect.x = pPlayer->playerX;
        flag->flagRect.y = pPlayer->playerY;
    }

        // Collision Check with walls
        checkCollisionWall(pPlayer, map);

        // Handle player input and movement for player 1
        handlePlayerInput(pPlayer, movementData->up, movementData->down, movementData->left, movementData->right, LEVEL_WIDTH, LEVEL_HEIGHT);

        // Handle player input and movement for player 2
        //handlePlayerInput(&playerRect2, &player2X, &player2Y, &player2VelocityX, &player2VelocityY, up2, down2, left2, right2, LEVEL_WIDTH, LEVEL_HEIGHT, playerRect2.w, playerRect2.h, SPEED);

        // Render players
        //renderVisibleMap(pRenderer, map, pPlayer, WINDOW_WIDTH, WINDOW_HEIGHT);
        renderGridMapCentered(pRenderer, map, pPlayer, WINDOW_WIDTH, WINDOW_HEIGHT, LEVEL_WIDTH, LEVEL_HEIGHT);
        flagAnimation(pRenderer, flag);
        for(int i=0; i<*numClients; i++){
            renderPlayer(pPlayer,pRenderer);
        }

        // Probably not needed here, but left it just in case I forget to use it :) - Konrad
        getPlayerGridPosition(pPlayer->playerX, pPlayer->playerY, &pPlayer->playerGridX, &pPlayer->playerGridY, map);

        // Present renderer
        SDL_RenderPresent(pRenderer);

        // Delay for frame rate control
        SDL_Delay(1000 / PLAYER_FRAME_RATE);
    }

    destroyPlayer(pPlayer);
    destroyFlag(flag);
    destroyGridMap(map);
    SDL_DestroyTexture(pPlayer->pPlayerTexture);
    SDL_DestroyTexture(flag->flagTexture);
    SDL_DestroyTexture(gridTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    printf("Adios amigos\n");
    SDL_Quit();
    return 0;
}
