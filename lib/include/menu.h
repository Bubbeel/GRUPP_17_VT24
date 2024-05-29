//
// Created by conne on 2024-04-29.
//

#ifndef GRUPP_17_VT24_MENU_H
#define GRUPP_17_VT24_MENU_H

#include <SDL2/SDL.h>
#include "music.h"


typedef struct {
    SDL_Surface* pointUISurface;
    SDL_Texture* pointUITexture;
    SDL_Rect pointUIRect;
    int currentPoints;
} GameUI;

int displayMenu(SDL_Window* pWindow, SDL_Renderer* pRenderer, int* joining);

GameUI* gameUI(SDL_Window* pWindow, SDL_Renderer* pRenderer);
GameUI* pointsDisplayUI(SDL_Window* pWindow, SDL_Renderer* pRenderer, int currentPoints);

void renderPointsUI(GameUI* pointsUI, SDL_Renderer* pRenderer);
int newDisplayUI(SDL_Renderer* pRenderer, GameUI* currentPointsUI, int currentPoints);

void destroyPointsUI(GameUI* pointsUI);
void destroyCurrentPointsUI(GameUI* currPointsUI);

#endif //GRUPP_17_VT24_MENU_H
