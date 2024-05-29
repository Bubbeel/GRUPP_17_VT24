#ifndef FLAG_H
#define FLAG_H

#include <SDL2/SDL.h>
#include "playerData.h"




typedef struct
{
    SDL_Surface* flagSurface;
    SDL_Texture* flagTexture;
    SDL_Rect flagRect;
    int flagFrames;
    int flagX, flagY;
    int xStart, yStart;
    Uint32 timeInterval;
}Flag;

Flag* createFlag(SDL_Renderer* renderer);
void flagAnimation(SDL_Renderer* renderer, Flag* flag, SDL_Rect camera); // with camera
void resetFlag(Flag* flag);
void destroyFlag(Flag* Flag);

#endif /* FLAG_H */