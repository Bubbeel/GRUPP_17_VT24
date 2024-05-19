#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "objects/player.h"
#include "objects/flag.h"
#include "objects/gridMap.h"
#include "objects/collisionDetection.h"

#define LEVEL_WIDTH 2816
#define LEVEL_HEIGHT 2100

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

#define CLOSE_DISTANCE_THRESHOLD 10
#define FLAG_SPEED 2


int main(int argc, char** argv) {
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
    //create GridMap obj and initialize GridMap
    //GridMap* map = createGridMap();
    GridMap* map = loadMapFromFile("resources/map.txt");
    SDL_Texture* gridTexture;
    gridTexture = loadGridMap(pRenderer);

    Player* pPlayer = createPlayer(pRenderer, map->cells[5][5].cellRect.x, map->cells[5][5].cellRect.y);
    if (!pPlayer) {
        printf("Failed to create player, Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    Flag* flag = createFlag(pRenderer);
    if(!flag)
    {
        printf("Failed to create flag, Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    //background data
    SDL_Surface* pSurface2 = IMG_Load("resources/Ship.png");

    //temp data storage, if you put anything here, please try to keep all the data in their specific files if possible

    //

    if (!pPlayer->playerSurface || !pSurface2 || !flag->flagSurface) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }
    
    SDL_Texture* pTexture2 = SDL_CreateTextureFromSurface(pRenderer, pSurface2);
    SDL_FreeSurface(pSurface2);
    if (!pPlayer->pPlayerTexture || !pTexture2 || !flag->flagTexture) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    //renderGridMap(pRenderer, map, gridTexture);

    SDL_Rect playerRect1;
    SDL_Rect playerRect2;
    SDL_QueryTexture(pTexture2, NULL, NULL, &playerRect2.w, &playerRect2.h);

    // playerRect2.w /= 20;
    // playerRect2.h /= 20;

    //Start positions
    // int player1X = playerRect1.w;
    // int player1Y = playerRect1.h; 

    int player2X = WINDOW_WIDTH - playerRect2.w; 
    int player2Y = WINDOW_HEIGHT - playerRect2.h;

    flag->flagRect.w /= 5;
    flag->flagRect.x = map->cells[20][20].cellRect.x;
    flag->flagRect.y = map->cells[20][20].cellRect.y;
    flag->flagX = flag->flagRect.x;
    flag->flagX = flag->flagRect.y;

    int player1VelocityX = 0;
    int player1VelocityY = 0;
    int player2VelocityX = 0;
    int player2VelocityY = 0;

    bool closeWindow = false;
    bool up1 = false, down1 = false, left1 = false, right1 = false;
    bool up2 = false, down2 = false, left2 = false, right2 = false;


    printf("Aloha amigos como estas\n");
    while (!closeWindow) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    closeWindow = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            up1 = true;
                            break;
                        case SDL_SCANCODE_A:
                            left1 = true;
                            break;
                        case SDL_SCANCODE_S:
                            down1 = true;
                            break;
                        case SDL_SCANCODE_D:
                            right1 = true;
                            break;
                        case SDL_SCANCODE_UP:
                            up2 = true;
                            break;
                        case SDL_SCANCODE_LEFT:
                            left2 = true;
                            break;
                        case SDL_SCANCODE_DOWN:
                            down2 = true;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            right2 = true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            up1 = false;
                            break;
                        case SDL_SCANCODE_A:
                            left1 = false;
                            break;
                        case SDL_SCANCODE_S:
                            down1 = false;
                            break;
                        case SDL_SCANCODE_D:
                            right1 = false;
                            break;
                        case SDL_SCANCODE_UP:
                            up2 = false;
                            break;
                        case SDL_SCANCODE_LEFT:
                            left2 = false;
                            break;
                        case SDL_SCANCODE_DOWN:
                            down2 = false;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            right2 = false;
                            break;
                    }
                    break;
            }
        }

    SDL_RenderClear(pRenderer);

    // Render Grid Map
    //renderGridMap(pRenderer, map, gridTexture);
    
    // Collision Check with the flag
    if (checkCollision(pPlayer, flag->flagRect))
    {
        flag->flagRect.x = pPlayer->playerX;
        flag->flagRect.y = pPlayer->playerY;
    }

    // Collision Check with walls
    checkCollisionWall(pPlayer, map);

    // Handle player input and movement for player 1 
    handlePlayerInput(pPlayer, up1, down1, left1, right1, LEVEL_WIDTH, LEVEL_HEIGHT);
    // printf("RectX, RectY: %d, %d\n", pPlayer->playerX, pPlayer->playerY);

    // Handle player input and movement for player 2
    //handlePlayerInput(&playerRect2, &player2X, &player2Y, &player2VelocityX, &player2VelocityY, up2, down2, left2, right2, LEVEL_WIDTH, LEVEL_HEIGHT, playerRect2.w, playerRect2.h, SPEED);

    // Render players
    //renderVisibleMap(pRenderer, map, pPlayer, WINDOW_WIDTH, WINDOW_HEIGHT);
    renderGridMapCentered(pRenderer, map, pPlayer, WINDOW_WIDTH, WINDOW_HEIGHT, LEVEL_WIDTH, LEVEL_HEIGHT);
    flagAnimation(pRenderer, flag);
    renderPlayer(pPlayer,pRenderer);

    SDL_RenderCopy(pRenderer, pTexture2, NULL, &playerRect2);

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
    SDL_DestroyTexture(pTexture2);
    SDL_DestroyTexture(flag->flagTexture);
    SDL_DestroyTexture(gridTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    printf("Adios amigos\n");
    SDL_Quit();
    return 0;
}