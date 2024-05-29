//
// Created by conne on 2024-04-29.
//

#include "menu.h"
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

// Function to display menu screen
int displayMenu(SDL_Window* pWindow, SDL_Renderer* pRenderer, int* joining) {
    // Load background image
    SDL_Surface* pSurfaceBackground = IMG_Load("../lib/resources/backgroundgame.png");
    if (!pSurfaceBackground) {
        printf("Error: %s\n", SDL_GetError());
        return -1; // Error
    }

    SDL_Texture* pTextureBackground = SDL_CreateTextureFromSurface(pRenderer, pSurfaceBackground);
    if (!pTextureBackground) {
        printf("Error: %s\n", SDL_GetError());
        SDL_FreeSurface(pSurfaceBackground);
        return -1; // Error
    }

    SDL_Rect backgroundRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    // Load button images
    SDL_Surface* pSurfaceStart = IMG_Load("../lib/resources/playbutton.png");
    SDL_Surface* pSurfaceQuit = IMG_Load("../lib/resources/exitbutton.jpg");

    if (!pSurfaceStart || !pSurfaceQuit) {
        printf("Error: %s\n", SDL_GetError());
        SDL_FreeSurface(pSurfaceStart);
        SDL_FreeSurface(pSurfaceQuit);
        SDL_DestroyTexture(pTextureBackground);
        return -1; // Error
    }

    SDL_Texture* pTextureStart = SDL_CreateTextureFromSurface(pRenderer, pSurfaceStart);
    SDL_Texture* pTextureQuit = SDL_CreateTextureFromSurface(pRenderer, pSurfaceQuit);

    if (!pTextureStart || !pTextureQuit) {
        printf("Error: %s\n", SDL_GetError());
        SDL_FreeSurface(pSurfaceStart);
        SDL_FreeSurface(pSurfaceQuit);
        SDL_DestroyTexture(pTextureBackground);
        SDL_DestroyTexture(pTextureStart);
        SDL_DestroyTexture(pTextureQuit);
        return -1; // Error
    }

    SDL_Rect startButtonRect = { (WINDOW_WIDTH - 200) / 2, 300, 200, 50 };
    SDL_Rect quitButtonRect = { (WINDOW_WIDTH - 200) / 2, 400, 200, 50 };

    int choice = 0; // 1 for start, 2 for quit

    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                choice = 2; // Quit if window closed
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (mouseX >= startButtonRect.x && mouseX <= startButtonRect.x + startButtonRect.w &&
                    mouseY >= startButtonRect.y && mouseY <= startButtonRect.y + startButtonRect.h) {
                    choice = 1; // Start button clicked
                    playMenuClickSound();
                    *joining = 1;
                    running = false;
                } else if (mouseX >= quitButtonRect.x && mouseX <= quitButtonRect.x + quitButtonRect.w &&
                           mouseY >= quitButtonRect.y && mouseY <= quitButtonRect.y + quitButtonRect.h) {
                    choice = 2; // Quit button clicked
                    playMenuClickSound();
                    running = false;
                }
            }
        }

        SDL_RenderCopy(pRenderer, pTextureBackground, NULL, &backgroundRect); // Render background image

        SDL_RenderCopy(pRenderer, pTextureStart, NULL, &startButtonRect); // Render start button
        SDL_RenderCopy(pRenderer, pTextureQuit, NULL, &quitButtonRect);   // Render quit button

        SDL_RenderPresent(pRenderer); // Update screen
    }

    SDL_FreeSurface(pSurfaceStart);
    SDL_FreeSurface(pSurfaceQuit);
    SDL_FreeSurface(pSurfaceBackground);
    SDL_DestroyTexture(pTextureStart);
    SDL_DestroyTexture(pTextureQuit);
    SDL_DestroyTexture(pTextureBackground);

    return choice;
}







GameUI* gameUI(SDL_Window* pWindow, SDL_Renderer* pRenderer) {
    GameUI* pointsUI = malloc(sizeof(GameUI));
    
    if (!pointsUI) 
    {
        return NULL;
    }

    pointsUI->pointUISurface = IMG_Load("../lib/resources/PointMenuWithText.png");
    if (!pointsUI->pointUISurface) {
        free(pointsUI);
        return NULL;
    }

    pointsUI->pointUITexture = SDL_CreateTextureFromSurface(pRenderer, pointsUI->pointUISurface);
    SDL_FreeSurface(pointsUI->pointUISurface);
    if (!pointsUI->pointUITexture) {
        free(pointsUI);
        return NULL;
    }

    pointsUI->pointUIRect.x = 25;
    pointsUI->pointUIRect.y = 25;
    SDL_QueryTexture(pointsUI->pointUITexture, NULL, NULL, &pointsUI->pointUIRect.w, &pointsUI->pointUIRect.h);
    
    return pointsUI;
}

GameUI* pointsDisplayUI(SDL_Window* pWindow, SDL_Renderer* pRenderer, int currentPoints) {
    GameUI* currentPointsUI = malloc(sizeof(GameUI));

    if (!currentPointsUI) 
    {
        return NULL;
    }
    switch(currentPoints) {
        case 0:
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number0.png"); // using nr 1, but should be 0
            break;
        case 1: 
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number1.png");
            break;
        case 2:
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number2.png");
            break;
        case 3:
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number3.png");
            break;
        default:
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number3.png");
            break;
    }
    if (!currentPointsUI->pointUISurface) {
        free(currentPointsUI);
        return NULL;
    }

    currentPointsUI->pointUITexture = SDL_CreateTextureFromSurface(pRenderer, currentPointsUI->pointUISurface);
    if (!currentPointsUI->pointUITexture) {
        free(currentPointsUI);
        return NULL;
    }

    currentPointsUI->pointUIRect.x = 178;
    currentPointsUI->pointUIRect.y = 55;
    SDL_QueryTexture(currentPointsUI->pointUITexture, NULL, NULL, &currentPointsUI->pointUIRect.w, &currentPointsUI->pointUIRect.h);
    return currentPointsUI;
}



void renderPointsUI(GameUI* pointsUI, SDL_Renderer* pRenderer) {
    SDL_RenderCopy(pRenderer, pointsUI->pointUITexture, NULL, &pointsUI->pointUIRect);
}



int newDisplayUI(SDL_Renderer* pRenderer, GameUI* currentPointsUI, int currentPoints){
    
    SDL_DestroyTexture(currentPointsUI->pointUITexture);

    if (!currentPointsUI) 
    {
        return 0;
    }
    switch(currentPoints) {
        case 0:
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number0.png"); // using nr 1, but should be 0
            break;
        case 1: 
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number1.png");
            break;
        case 2:
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number2.png");
            break;
        case 3:
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number3.png");
            break;
        default:
            currentPointsUI->pointUISurface = IMG_Load("../lib/resources/number3.png");
            break;
    }

    if (!currentPointsUI->pointUISurface) {
        free(currentPointsUI);
        return 0;
    }

    currentPointsUI->pointUITexture = SDL_CreateTextureFromSurface(pRenderer, currentPointsUI->pointUISurface);
    if (!currentPointsUI->pointUITexture) {
        free(currentPointsUI);
        return 0;
    }

    currentPointsUI->pointUIRect.x = 178;
    currentPointsUI->pointUIRect.y = 55;
    SDL_QueryTexture(currentPointsUI->pointUITexture, NULL, NULL, &currentPointsUI->pointUIRect.w, &currentPointsUI->pointUIRect.h);
    renderPointsUI(currentPointsUI, pRenderer);
    return 1;
}  




void destroyPointsUI(GameUI* pointsUI){
    SDL_DestroyTexture(pointsUI->pointUITexture);
    free(pointsUI);
}



void destroyCurrentPointsUI(GameUI* currPointsUI){
    SDL_DestroyTexture(currPointsUI->pointUITexture);
    free(currPointsUI);
}