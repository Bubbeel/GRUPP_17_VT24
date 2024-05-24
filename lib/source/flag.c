#include "playerData.h"
#include "flag.h"
#include "player.h"
#include <math.h>
#include <SDL2/SDL_image.h>
#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

Flag* createFlag(SDL_Renderer* renderer,int startPosX, int startPoxY) 
{
    Flag* flag = malloc(sizeof(flag));
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
    flag->flagX = startPosX;
    flag->flagY = startPoxY;
    flag->flagRect.x = flag->flagX;
    flag->flagRect.y = flag->flagY;
    flag->flagFrames = 0;
    flag->camera.x = 0;
    flag->camera.y = 0;
    flag->camera.h = WINDOW_HEIGHT;
    flag->camera.w = WINDOW_WIDTH;
    return flag;
}

//Doesn't work currently, don't know why
void flagAnimation(Flag* flag, SDL_Renderer* renderer)
{
    flag->flagRect.x = flag->flagX - flag->camera.x;
    flag->flagRect.y = flag->flagY - flag->camera.y;
    SDL_RenderCopy(renderer, flag->flagTexture, NULL, &flag->flagRect);
}

void destroyFlag(Flag* Flag) 
{
    SDL_DestroyTexture(Flag->flagTexture);
    free(Flag);
}


// Keeping this function commented in case we need it/something similar to it - Konrad
void moveFlag(SDL_Rect *flagRect, int player1X, int player1Y, int closeDistanceThreshold, int flagSpeed) {
    int distToPlayer1 = sqrt(pow(player1X - flagRect->x, 2) + pow(player1Y - flagRect->y, 2));
    if (distToPlayer1 < closeDistanceThreshold) {
        flagRect->x = player1X;
        flagRect->y = player1Y;
    }
    else {
        // Move flag towards player1
        if (player1X > flagRect->x) {
            flagRect->x += flagSpeed;
        }
        else {
            flagRect->x -= flagSpeed;
        }

        if (player1Y > flagRect->y) {
            flagRect->y += flagSpeed;
        }
        else {
            flagRect->y -= flagSpeed;
        }
    }
}

void getFlagSendData(Flag* flag, FlagData* flagData) {
    flagData->x = flag->flagX;
    flagData->y = flag->flagY;
}

void updateFlagWithRecievedData(Flag* flag, FlagData* flagData) {
    flag->flagX = flagData->x;
    flag->flagY = flagData->x;
}