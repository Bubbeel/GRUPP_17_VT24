#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "map.h"

#define SPEED 100
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

//renderBackground initialization for GridMap
void renderBackground(SDL_Renderer *gRenderer, SDL_Texture *mTile, SDL_Rect gTiles[]);
//dunno if this helps
bool init(SDL_Renderer **gRenderer);
void loadMedia(SDL_Renderer *gRenderer, SDL_Texture **mSpaceman, SDL_Rect gSpriteClips[], SDL_Texture **mAlien, SDL_Rect gAlien[], SDL_Texture **mTiles, SDL_Rect gTiles[]);

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

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

    //background data
    SDL_Texture *mTiles = NULL;
    SDL_Rect gTiles[16];

    // if (init(&gRenderer)) {
    //     printf("worked\n");
    // }
    
    //loadMedia(gRenderer, &mSpaceman, gSpriteClips, &mAlien, gAlien, &mTiles, gTiles);

    //renderBackground
    renderBackground(pRenderer, mTiles, gTiles);

    SDL_Surface* pSurface1 = IMG_Load("resources/player1.png");
    SDL_Surface* pSurface2 = IMG_Load("resources/player2.png");

    if (!pSurface1 || !pSurface2) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* pTexture1 = SDL_CreateTextureFromSurface(pRenderer, pSurface1);
    SDL_Texture* pTexture2 = SDL_CreateTextureFromSurface(pRenderer, pSurface2);
    SDL_FreeSurface(pSurface1);
    SDL_FreeSurface(pSurface2);

    if (!pTexture1 || !pTexture2) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Rect playerRect1;
    SDL_Rect playerRect2;
    SDL_QueryTexture(pTexture1, NULL, NULL, &playerRect1.w, &playerRect1.h);
    SDL_QueryTexture(pTexture2, NULL, NULL, &playerRect2.w, &playerRect2.h);

    //storlek
    playerRect1.w /= 20;
    playerRect1.h /= 20;
    playerRect2.w /= 20;
    playerRect2.h /= 20;

    // Startpositioner:

    //övre vänster kant
    float player1X = playerRect1.w;
    float player1Y = playerRect1.h; 

    //nedre höger kant
    float player2X = WINDOW_WIDTH - playerRect2.w; 
    float player2Y = WINDOW_HEIGHT - playerRect2.h;

    float player1VelocityX = 0;
    float player1VelocityY = 0;
    float player2VelocityX = 0;
    float player2VelocityY = 0;
    
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
                        //player1 styrning
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
                        // Spelare 2 styrning 
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
                        // Spelare 1 styrning
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
                        // Spelare 2 styrning
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
        // Spelare 1 rörelse
        player1VelocityX = player1VelocityY = 0;
        if (up1 && !down1) {
            player1VelocityY = -SPEED;
            /*SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "uppies")*/;
            printf("uppies");
        }
        if (down1 && !up1) player1VelocityY = SPEED;
        if (left1 && !right1) player1VelocityX = -SPEED;
        if (right1 && !left1) player1VelocityX = SPEED;
        player1X += player1VelocityX / 60; // 60 frames/s
        player1Y += player1VelocityY / 60;
        if (player1X < 0) player1X = 0;
        if (player1Y < 0) player1Y = 0;
        if (player1X > WINDOW_WIDTH - playerRect1.w) player1X = WINDOW_WIDTH - playerRect1.w;
        if (player1Y > WINDOW_HEIGHT - playerRect1.h) player1Y = WINDOW_HEIGHT - playerRect1.h;
        playerRect1.x = player1X;
        playerRect1.y = player1Y;

        // Spelare 2 rörelse
        player2VelocityX = player2VelocityY = 0;
        if (up2 && !down2) player2VelocityY = -SPEED;
        if (down2 && !up2) player2VelocityY = SPEED;
        if (left2 && !right2) player2VelocityX = -SPEED;
        if (right2 && !left2) player2VelocityX = SPEED;
        player2X += player2VelocityX / 60; // 60 frames/s
        player2Y += player2VelocityY / 60;
        if (player2X < 0) player2X = 0;
        if (player2Y < 0) player2Y = 0;
        if (player2X > WINDOW_WIDTH - playerRect2.w) player2X = WINDOW_WIDTH - playerRect2.w;
        if (player2Y > WINDOW_HEIGHT - playerRect2.h) player2Y = WINDOW_HEIGHT - playerRect2.h;
        playerRect2.x = player2X;
        playerRect2.y = player2Y;

        SDL_RenderClear(pRenderer);
        SDL_RenderCopy(pRenderer, pTexture1, NULL, &playerRect1);
        SDL_RenderCopy(pRenderer, pTexture2, NULL, &playerRect2);
        SDL_RenderPresent(pRenderer);
        SDL_Delay(1000 / 60); // 60 frames/s
    }

    SDL_DestroyTexture(pTexture1);
    SDL_DestroyTexture(pTexture2);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    SDL_Quit();
    return 0;
}

//renderBackground for GridMap
void renderBackground(SDL_Renderer *gRenderer, SDL_Texture *mTiles, SDL_Rect gTiles[]){
    
    SDL_Rect possition;
    possition.y = 0;
    possition.x = 0;
    possition.h = getTileHeight();
    possition.w = getTileWidth();
    
    for (int i = 0; i<getTileColumns(); i++) {
        for (int j = 0; j<getTileRows(); j++) {
            possition.y = i*getTileHeight();
            possition.x = j*getTileWidth();
            SDL_RenderCopyEx(gRenderer, mTiles, &gTiles[getTileGrid(i,j)],&possition , 0, NULL, SDL_FLIP_NONE);
        }
    }
}

//loadmedia + init
void loadMedia(SDL_Renderer *gRenderer, SDL_Texture **mSpaceman, SDL_Rect gSpriteClips[], SDL_Texture **mAlien, SDL_Rect gAlien[], SDL_Texture **mTiles, SDL_Rect gTiles[]){
    
    SDL_Surface* gSpacemanSurface = IMG_Load("resources/SPACEMAN.PNG");
    *mSpaceman = SDL_CreateTextureFromSurface(gRenderer, gSpacemanSurface);
  
    
    gSpriteClips[ 0 ].x =   0;
    gSpriteClips[ 0 ].y =   0;
    gSpriteClips[ 0 ].w =  16;
    gSpriteClips[ 0 ].h = 16;
    
    gSpriteClips[ 1 ].x =  16;
    gSpriteClips[ 1 ].y =   0;
    gSpriteClips[ 1 ].w =  16;
    gSpriteClips[ 1 ].h = 16;
    
    gSpriteClips[ 2 ].x = 32;
    gSpriteClips[ 2 ].y =   0;
    gSpriteClips[ 2 ].w =  16;
    gSpriteClips[ 2 ].h = 16;
    
    gSpriteClips[ 3 ].x = 48;
    gSpriteClips[ 3 ].y =   0;
    gSpriteClips[ 3 ].w =  16;
    gSpriteClips[ 3 ].h = 16;
    
    gSpriteClips[ 4 ].x = 64;
    gSpriteClips[ 4 ].y =   0;
    gSpriteClips[ 4 ].w =  16;
    gSpriteClips[ 4 ].h = 16;
    
    gSpriteClips[ 5 ].x = 80;
    gSpriteClips[ 5 ].y =   0;
    gSpriteClips[ 5 ].w =  16;
    gSpriteClips[ 5 ].h = 16;
    
    gSpriteClips[ 6 ].x = 96;
    gSpriteClips[ 6 ].y =   0;
    gSpriteClips[ 6 ].w =  16;
    gSpriteClips[ 6 ].h = 16;
    
    gSpriteClips[ 7 ].x = 112;
    gSpriteClips[ 7 ].y =   0;
    gSpriteClips[ 7 ].w =  16;
    gSpriteClips[ 7 ].h = 16;
    
    
    SDL_Surface* gTilesSurface = IMG_Load("resources/TILES.PNG");
    *mTiles = SDL_CreateTextureFromSurface(gRenderer, gTilesSurface);
    for (int i = 0; i < 16; i++) {
        gTiles[i].x = i*getTileWidth();
        gTiles[i].y = 0;
        gTiles[i].w = getTileWidth();
        gTiles[i].h = getTileHeight();
    }
    
    SDL_Surface* gSAlien = IMG_Load("resources/ALIEN.PNG");
    *mAlien = SDL_CreateTextureFromSurface(gRenderer, gSAlien);
    gAlien[ 0 ].x = 0;
    gAlien[ 0 ].y = 0;
    gAlien[ 0 ].w = 15;
    gAlien[ 0 ].h = 15;
    
    gAlien[ 1 ].x = 15;
    gAlien[ 1 ].y = 0;
    gAlien[ 1 ].w = 15;
    gAlien[ 1 ].h = 15;
    
    
}


bool init(SDL_Renderer **gRenderer){
    bool test = true;
    SDL_Window  *gWindow = NULL;
    SDL_Init(SDL_INIT_VIDEO);
    gWindow = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, 400, 300, SDL_WINDOW_SHOWN);
    if(gWindow == NULL){
        printf("Fungerar ej\n");
        test = false;
    }
    *gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
    if(*gRenderer == NULL){
        printf("Fungerar ej\n");
        test = false;
    }
    return test;
    
}