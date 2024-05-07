//
// Created by conne on 2024-04-29.
//

#include "../objects/menu.h"
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Function to display menu screen
// Function to display menu screen
// Function to display menu screen
int displayMenu(SDL_Window* pWindow, SDL_Renderer* pRenderer) {
    // Load background image
    SDL_Surface* pSurfaceBackground = IMG_Load("resources/backgroundgame.png");
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
    SDL_Surface* pSurfaceStart = IMG_Load("resources/playbutton.png");
    SDL_Surface* pSurfaceQuit = IMG_Load("resources/exitbutton.jpg");
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

    // Define button rectangles
    SDL_Rect startButtonRect = { (WINDOW_WIDTH - 200) / 2, (WINDOW_HEIGHT - 50) / 2, 200, 50 };
    SDL_Rect quitButtonRect = { (WINDOW_WIDTH - 200) / 2, (WINDOW_HEIGHT - 50) / 2 + 100, 200, 50 };

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

        // Render background image
        SDL_RenderCopy(pRenderer, pTextureBackground, NULL, &backgroundRect);

        // Render buttons
        SDL_RenderCopy(pRenderer, pTextureStart, NULL, &startButtonRect);
        SDL_RenderCopy(pRenderer, pTextureQuit, NULL, &quitButtonRect);

        // Present renderer
        SDL_RenderPresent(pRenderer);
    }

    // Free resources
    SDL_FreeSurface(pSurfaceBackground);
    SDL_FreeSurface(pSurfaceStart);
    SDL_FreeSurface(pSurfaceQuit);
    SDL_DestroyTexture(pTextureBackground);
    SDL_DestroyTexture(pTextureStart);
    SDL_DestroyTexture(pTextureQuit);

    return choice;
}


// Function to display game over screen