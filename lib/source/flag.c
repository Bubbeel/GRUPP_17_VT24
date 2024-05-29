#include <SDL2/SDL_image.h>
#include "flag.h"
#include <math.h>

Flag* createFlag(SDL_Renderer* renderer) 
{
    Flag* flag = malloc(sizeof(Flag));
    if (!flag) 
    {
        return NULL;
    }

    flag->flagSurface = IMG_Load("../lib/resources/flag.png");
    if (!flag->flagSurface) {
        free(flag);
        return NULL;
    }

    flag->flagTexture = SDL_CreateTextureFromSurface(renderer, flag->flagSurface);
    SDL_FreeSurface(flag->flagSurface);
    if (!flag->flagTexture) {
        free(flag);
        return NULL;
    }

    SDL_QueryTexture(flag->flagTexture, NULL, NULL, &flag->flagRect.w, &flag->flagRect.h);

    // Initialize flag frames
    flag->flagFrames = 0;
    flag->flagRect.w /= 5;
    flag->xStart = 0;
    flag->yStart = 0;

    return flag;
}

void flagAnimation(SDL_Renderer* renderer, Flag* flag, SDL_Rect camera)
{


    SDL_Rect flagsRect = {
        flag->flagRect.x - camera.x,
        flag->flagRect.y - camera.y,
        flag->flagRect.w,
        flag->flagRect.h
    };

    SDL_Rect srcRect = { flag->flagFrames * flag->flagRect.w, 0, flag->flagRect.w, flag->flagRect.h };
    SDL_RenderCopy(renderer, flag->flagTexture, &srcRect, &flagsRect);
    flag->flagFrames = (flag->flagFrames + 1) % 5;
}


void resetFlag(Flag* flag){
    flag->flagRect.x = flag->xStart;
    flag->flagRect.y = flag->yStart;
    flag->flagX = flag->flagRect.x;
    flag->flagY = flag->flagRect.y;
}

void destroyFlag(Flag* Flag) 
{
    SDL_DestroyTexture(Flag->flagTexture);
    free(Flag);
}