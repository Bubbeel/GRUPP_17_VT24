//
// Created by conne on 2024-04-29.
//

#include "menu.h"
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Function to display menu screen
int displayMenu(SDL_Window* pWindow, SDL_Renderer* pRenderer) {
    SDL_Surface* pSurfaceStart = IMG_Load("resources/playbutton.png");
    SDL_Surface* pSurfaceQuit = IMG_Load("resources/exitbutton.jpg");

    if (!pSurfaceStart || !pSurfaceQuit) {
        printf("Error: %s\n", SDL_GetError());
        SDL_FreeSurface(pSurfaceStart);
        SDL_FreeSurface(pSurfaceQuit);
        return -1; // Error
    }

    SDL_Texture* pTextureStart = SDL_CreateTextureFromSurface(pRenderer, pSurfaceStart);
    SDL_Texture* pTextureQuit = SDL_CreateTextureFromSurface(pRenderer, pSurfaceQuit);

    if (!pTextureStart || !pTextureQuit) {
        printf("Error: %s\n", SDL_GetError());
        SDL_FreeSurface(pSurfaceStart);
        SDL_FreeSurface(pSurfaceQuit);
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
                    running = false;
                } else if (mouseX >= quitButtonRect.x && mouseX <= quitButtonRect.x + quitButtonRect.w &&
                           mouseY >= quitButtonRect.y && mouseY <= quitButtonRect.y + quitButtonRect.h) {
                    choice = 2; // Quit button clicked
                    running = false;
                }
            }
        }

        SDL_RenderClear(pRenderer);

        SDL_RenderCopy(pRenderer, pTextureStart, NULL, &startButtonRect);
        SDL_RenderCopy(pRenderer, pTextureQuit, NULL, &quitButtonRect);

        SDL_RenderPresent(pRenderer);
    }

    SDL_FreeSurface(pSurfaceStart);
    SDL_FreeSurface(pSurfaceQuit);
    SDL_DestroyTexture(pTextureStart);
    SDL_DestroyTexture(pTextureQuit);

    return choice;
}


// Function to display game over screen