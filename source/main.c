#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "objects/player.h"
#include "objects/flag.h"

#define SPEED 100
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

#define CLOSE_DISTANCE_THRESHOLD 200
#define FLAG_SPEED 2

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    int flagX, flagY;

    SDL_Window* pWindow = SDL_CreateWindow("Enkelt exempel 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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

    SDL_Surface* pSurface1 = IMG_Load("resources/player1.png");
    SDL_Surface* pSurface2 = IMG_Load("resources/player2.png");
    SDL_Surface* pSurfaceFlag = IMG_Load("resources/spritesheet.png");

    if (!pSurface1 || !pSurface2 || !pSurfaceFlag) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* pTexture1 = SDL_CreateTextureFromSurface(pRenderer, pSurface1);
    SDL_Texture* pTexture2 = SDL_CreateTextureFromSurface(pRenderer, pSurface2);
    SDL_Texture* pTextureFlag = SDL_CreateTextureFromSurface(pRenderer, pSurfaceFlag);
    SDL_FreeSurface(pSurface1);
    SDL_FreeSurface(pSurface2);
    SDL_FreeSurface(pSurfaceFlag);

    if (!pTexture1 || !pTexture2 || !pTextureFlag) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Rect playerRect1;
    SDL_Rect playerRect2;
    SDL_Rect flagRect;
    SDL_QueryTexture(pTexture1, NULL, NULL, &playerRect1.w, &playerRect1.h);
    SDL_QueryTexture(pTexture2, NULL, NULL, &playerRect2.w, &playerRect2.h);
    SDL_QueryTexture(pTextureFlag, NULL, NULL, &flagRect.w, &flagRect.h);

    playerRect1.w /= 20;
    playerRect1.h /= 20;
    playerRect2.w /= 20;
    playerRect2.h /= 20;
    flagRect.w /= 5;

    float player1X = playerRect1.w;
    float player1Y = playerRect1.h; 

    float player2X = WINDOW_WIDTH - playerRect2.w; 
    float player2Y = WINDOW_HEIGHT - playerRect2.h;

    flagRect.x = (WINDOW_WIDTH - flagRect.w) / 2;
    flagRect.y = (WINDOW_HEIGHT - flagRect.h) / 2;

    float player1VelocityX = 0;
    float player1VelocityY = 0;
    float player2VelocityX = 0;
    float player2VelocityY = 0;

    int flagFrame = 0;

    bool closeWindow = false;
    bool up1, down1, left1, right1;
    bool up2, down2, left2, right2;

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

    // Clear the renderer
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pRenderer);

    // Update flag animation frame
    SDL_Rect srcRect = { flagFrame * flagRect.w, 0, flagRect.w, flagRect.h };
    SDL_RenderCopy(pRenderer, pTextureFlag, &srcRect, &flagRect);

    // Move flag logic...
    moveFlag(&flagRect, player1X, player1Y, player2X, player2Y, CLOSE_DISTANCE_THRESHOLD, FLAG_SPEED);

    // Increment flag frame
    flagFrame = (flagFrame + 1) % 5;

    // Handle player input and movement for player 1
    handlePlayerInput(&playerRect1, &player1X, &player1Y, &player1VelocityX, &player1VelocityY, up1, down1, left1, right1, WINDOW_WIDTH, WINDOW_HEIGHT, playerRect1.w, playerRect1.h, SPEED);

    // Handle player input and movement for player 2
    handlePlayerInput(&playerRect2, &player2X, &player2Y, &player2VelocityX, &player2VelocityY, up2, down2, left2, right2, WINDOW_WIDTH, WINDOW_HEIGHT, playerRect2.w, playerRect2.h, SPEED);

    // Render players
    SDL_RenderCopy(pRenderer, pTexture1, NULL, &playerRect1);
    SDL_RenderCopy(pRenderer, pTexture2, NULL, &playerRect2);

    // Present renderer
    SDL_RenderPresent(pRenderer);

    // Delay for frame rate control
    SDL_Delay(1000 / PLAYER_FRAME_RATE);
}


    SDL_DestroyTexture(pTexture1);
    SDL_DestroyTexture(pTexture2);
    SDL_DestroyTexture(pTextureFlag);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    SDL_Quit();
    return 0;
}
