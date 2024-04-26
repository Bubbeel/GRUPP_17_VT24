#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "collisionDetection.h"

int checkCollision(SDL_Rect a, SDL_Rect b)
{
    int leftA, rightA, topA, bottomA;
    int leftB, rightB, topB, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    if( bottomA <= topB )
    {
        printf("Collision not detected\n");
        return 0;
    }

    if( topA >= bottomB )
    {
        printf("Collision not detected\n");
        return 0;
    }

    if( rightA <= leftB )
    {
        printf("Collision not detected\n");
        return 0;
    }

    if( leftA >= rightB )
    {
        printf("Collision not detected\n");
        return 0;
    }
    printf("Collision detected\n");
    return 1;
}