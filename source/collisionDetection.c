#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "objects/collisionDetection.h"
#include "objects/gridMap.h"
#include "objects/player.h"

bool checkCollision(SDL_Rect a, SDL_Rect b)
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
        //printf("Collision not detected\n");
        return false;
    }

    if( topA >= bottomB )
    {
        //printf("Collision not detected\n");
        return false;
    }

    if( rightA <= leftB )
    {
        //printf("Collision not detected\n");
        return false;
    }

    if( leftA >= rightB )
    {
        //printf("Collision not detected\n");
        return false;
    }
    
    //printf("Collision detected\n");
    return true;
}

void checkCollisionWall(Player* player, GridMap* map)
{
    for (int y = 0; y < GRID_HEIGHT; y++) 
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            if (strcmp(map->cells[y][x].tag, "Wall") == 0)
            {
                if(checkCollision(player->playerRect, map->cells[y][x].cellRect) == true)
                {
                    printf("playerRectx: %d, playerRecty: %d, mapcellrect.x: %d, mapcellrect.y: %d\n", player->playerRect.x, player->playerRect.y, map->cells[y][x].cellRect.x, map->cells[y][x].cellRect.y);
                    // Current problem: when we go below the mid-point the collision doesnt work
                    // Push the player back
                    //player->playerX = player->playerRect.x -= player->playerVelocityX / 60;
                    //player->playerY = player->playerRect.y -= player->playerVelocityY / 60;
                }
            }
        }
    }
} 