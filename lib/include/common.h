#ifndef COMMON_H
#define COMMON_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "../include/player.h"

typedef struct{
    int x, y;
    int clientId;
}PlayerPackage;

typedef struct
{
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Surface surface;
    SDL_Renderer *renderer;
    char tag;
} GameObject;

typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
} PlayerMovementData;

#endif