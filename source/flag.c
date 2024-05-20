#include <SDL2/SDL_image.h>
#include "../objects/flag.h"
#include <math.h>

Flag* createFlag(SDL_Renderer* renderer) 
{
    Flag* Flag = malloc(sizeof(Flag));
    if (!Flag) 
    {
        return NULL;
    }

    Flag->flagSurface = IMG_Load("resources/flag.png");
    if (!Flag->flagSurface) {
        free(Flag);
        return NULL;
    }

    Flag->flagTexture = SDL_CreateTextureFromSurface(renderer, Flag->flagSurface);
    SDL_FreeSurface(Flag->flagSurface);
    if (!Flag->flagTexture) {
        free(Flag);
        return NULL;
    }

    SDL_QueryTexture(Flag->flagTexture, NULL, NULL, &Flag->flagRect.w, &Flag->flagRect.h);

    return Flag;
}

//Doesn't work currently, don't know why
void flagAnimation(SDL_Renderer* renderer, Flag* flag)
{
    const int NUM_FRAMES = 5;

    int frameWidth = flag->flagRect.w / NUM_FRAMES;

    SDL_Rect srcRect = { flag->flagFrames * frameWidth, 0, frameWidth, flag->flagRect.h };

    SDL_RenderCopy(renderer, flag->flagTexture, &srcRect, &flag->flagRect);

    flag->flagFrames = (flag->flagFrames + 1) % NUM_FRAMES;
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