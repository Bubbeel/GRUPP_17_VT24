#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define SPEED 100
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

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

    //storlek
    playerRect1.w /= 20;
    playerRect1.h /= 20;
    playerRect2.w /= 20;
    playerRect2.h /= 20;
    flagRect.w /= 5;

    // Startpositioner:

    //övre vänster kant
    float player1X = playerRect1.w;
    float player1Y = playerRect1.h; 

    //nedre höger kant
    float player2X = WINDOW_WIDTH - playerRect2.w; 
    float player2Y = WINDOW_HEIGHT - playerRect2.h;

    // flagga i mitten
    flagRect.x = (WINDOW_WIDTH - flagRect.w) / 2;
    flagRect.y = (WINDOW_HEIGHT - flagRect.h) / 2;

    float player1VelocityX = 0;
    float player1VelocityY = 0;
    float player2VelocityX = 0;
    float player2VelocityY = 0;

    //flaggans framerate
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


        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pRenderer);

        SDL_Rect srcRect = { flagFrame * flagRect.w, 0, flagRect.w, flagRect.h };
        SDL_RenderCopy(pRenderer, pTextureFlag, &srcRect, &flagRect);
        SDL_RenderPresent(pRenderer);

        flagFrame = (flagFrame + 1) % 5;

        // Spelare 1 rörelse
        player1VelocityX = player1VelocityY = 0;
        if (up1 && !down1) player1VelocityY = -SPEED;
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


        SDL_RenderCopy(pRenderer, pTexture1, NULL, &playerRect1);
        SDL_RenderCopy(pRenderer, pTexture2, NULL, &playerRect2);
        SDL_RenderPresent(pRenderer);
        SDL_Delay(1000 / PLAYER_FRAME_RATE); // 60 frames/s


    }

    SDL_DestroyTexture(pTexture1);
    SDL_DestroyTexture(pTexture2);
    SDL_DestroyTexture(pTextureFlag); // flagga
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);


    SDL_Quit();
    return 0;
}
