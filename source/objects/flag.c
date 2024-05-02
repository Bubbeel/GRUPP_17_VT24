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

    flag->flagSurface = IMG_Load("resources/flag.png");
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

    return flag;
}

void flagAnimation(SDL_Renderer* renderer, Flag* flag)
{
    SDL_Rect srcRect = { flag->flagFrames * flag->flagRect.w, 0, flag->flagRect.w, flag->flagRect.h };
    SDL_RenderCopy(renderer, flag->flagTexture, &srcRect, &flag->flagRect);
    flag->flagFrames = (flag->flagFrames + 1) % 5;
}

void destroyFlag(Flag* Flag) 
{
    SDL_DestroyTexture(Flag->flagTexture);
    free(Flag);
}





// Keeping this function commented in case we need it/something similar to it - Konrad
// void moveFlag(SDL_Rect *flagRect, int player1X, int player1Y, int player2X, int player2Y, int closeDistanceThreshold, int flagSpeed) {

//     int distToPlayer1 = sqrt(pow(player1X - flagRect->x, 2) + pow(player1Y - flagRect->y, 2));

//     int distToPlayer2 = sqrt(pow(player2X - flagRect->x, 2) + pow(player2Y - flagRect->y, 2));

//     if (distToPlayer1 < closeDistanceThreshold && distToPlayer1 < distToPlayer2) {
//         flagRect->x = player1X;
//         flagRect->y = player1Y;
//     }
 
//     if (distToPlayer2 < closeDistanceThreshold && distToPlayer2 < distToPlayer1) {
//         flagRect->x = player2X;
//         flagRect->y = player2Y;
//     }
// }