#ifndef FLAG_H
#define FLAG_H
#include "playerData.h"
#include "player.h"
#include <SDL2/SDL.h>

struct flag
{
    SDL_Surface* flagSurface;
    SDL_Texture* flagTexture;
    SDL_Rect flagRect;
    int flagFrames;
    SDL_Rect camera;
    int flagX, flagY;
};
typedef struct flag Flag;

Flag* createFlag(SDL_Renderer* renderer, int startPosX, int startPoxY);
void flagAnimation(Flag* flag, SDL_Renderer* renderer);
void destroyFlag(Flag* Flag);

void getFlagSendData(Flag* flag, FlagData* flagData);
void updateFlagWithRecievedData(Flag* flag, FlagData* flagData);
void moveFlag(SDL_Rect *flagRect, int player1X, int player1Y, int closeDistanceThreshold, int flagSpeed);

#endif /* FLAG_H */