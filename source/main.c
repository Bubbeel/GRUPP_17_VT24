#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "objects/player.h"
#include "objects/flag.h"
#include "gridMap.h"
#include "collisionDetection.h"

// Player Speed is now in the player.c file

#define LEVEL_WIDTH 2816
#define LEVEL_HEIGHT 1600

#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

#define CLOSE_DISTANCE_THRESHOLD 10
#define FLAG_SPEED 2

// WIP - Konrad
typedef struct
{
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Surface surface;
    char tag;
} GameObject;

SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};


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

    Player* pPlayer = createPlayer(pRenderer);
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
    int* tempX;
    int* tempY;
    //

    //create GridMap obj and initialize GridMap
    GridMap* map = createGridMap();
    //CHECK THE FILE TO MAKE SURE EVERYTHING IS OKAY
    loadMapFromFile("resources/map.txt", map);
    SDL_Texture* gridTexture;
    gridTexture = loadGridMap(pRenderer);

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

    playerRect2.w /= 20;
    playerRect2.h /= 20;

    //Start positions
    int player1X = playerRect1.w;
    int player1Y = playerRect1.h; 

    int player2X = WINDOW_WIDTH - playerRect2.w; 
    int player2Y = WINDOW_HEIGHT - playerRect2.h;

    flag->flagRect.w /= 5;
    flag->flagRect.x = WINDOW_WIDTH / 2;
    flag->flagRect.y = WINDOW_HEIGHT / 2;
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
    renderVisibleMap(pRenderer, map, pPlayer->playerX, pPlayer->playerY, WINDOW_WIDTH, WINDOW_HEIGHT);

    // camera.x = (pPlayer->playerX + pPlayer->playerRect.w/2) - WINDOW_WIDTH/2;
    // camera.y = (pPlayer->playerY + pPlayer->playerRect.h/2) - WINDOW_HEIGHT/2;
    // //printf("playervelx: %d , playervely: %d \n",pPlayer->playerVelocityX, pPlayer->playerVelocityY);
    // if( camera.x < 0 )
    // { 
    //    camera.x = 0;
    // }
    // if( camera.y < 0 )
    // {
    //     camera.y = 0;
    // }
    // if( camera.x > WINDOW_WIDTH - camera.w )
    // {
    //     camera.x = WINDOW_WIDTH - camera.w;
    // }
    // if( camera.y > WINDOW_HEIGHT - camera.h )
    // {
    //     camera.y = WINDOW_HEIGHT - camera.h;
    // }
    
    // Collision Check with the flag
    if (checkCollision(pPlayer->playerRect, flag->flagRect))
    {
        flag->flagRect.x = pPlayer->playerX;
        flag->flagRect.y = pPlayer->playerY;
    }

    flagAnimation(pRenderer, flag);

    // Handle player input and movement for player 1 
    handlePlayerInput(pPlayer, up1, down1, left1, right1, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Handle player input and movement for player 2
    //handlePlayerInput(&playerRect2, &player2X, &player2Y, &player2VelocityX, &player2VelocityY, up2, down2, left2, right2, LEVEL_WIDTH, LEVEL_HEIGHT, playerRect2.w, playerRect2.h, SPEED);

    // Render players
    renderPlayer(pPlayer,pRenderer);
    SDL_RenderCopy(pRenderer, pTexture2, NULL, &playerRect2);



    //printf("FlagX: %d, FlagY: %d\n", flag->flagX, flag->flagY);
    // Probably not needed here, but left it just in case I forget to use it :) - Konrad
    getPlayerGridPosition(pPlayer->playerX, pPlayer->playerY, &pPlayer->playerGridX, &pPlayer->playerGridY, map);
    //getPlayerGridPosition(flag->flagX, flag->flagY, tempX, tempY);

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