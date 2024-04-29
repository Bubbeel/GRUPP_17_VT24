#ifndef FLAG_H
#define FLAG_H

#include <SDL2/SDL.h>

typedef struct
{
    SDL_Surface* flagSurface;
    SDL_Texture* flagTexture;
    SDL_Rect flagRect;
    int flagFrames;
    int flagX, flagY;
}Flag;

Flag* createFlag(SDL_Renderer* renderer);
void flagAnimation(SDL_Renderer* renderer, Flag* flag);
void destroyFlag(Flag* Flag);

//void moveFlag(SDL_Rect *flagRect, int player1X, int player1Y, int player2X, int player2Y, int closeDistanceThreshold, int flagSpeed);

#endif /* FLAG_H */