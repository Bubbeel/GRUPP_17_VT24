#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "collisionDetection.h"
#include "gridmap.h"
#include "player.h"

Collision checkCollision(Player* player, SDL_Rect b)
{
    // Calculate the player's movement direction

    int dx = player->playerVelocityX / 60;
    int dy = player->playerVelocityY / 60;

    int leftA, rightA, topA, bottomA;
    int leftB, rightB, topB, bottomB;

    Collision collision = noCollision;

    bool topCol = false;
    bool botCol = false;
    bool rightCol = false;
    bool leftCol = false;
    //Calculate the sides of rect A

    leftA = player->playerX;
    rightA = player->playerX + player->playerRect.w;
    topA = player->playerY;
    bottomA = player->playerY + player->playerRect.h;

    //Calculate the sides of rect B

    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    // if (bottomA <= topB || topA >= bottomB || rightA <= leftB || leftA >= rightB) {
    //     return noCollision;
    // } dont need this i dont think

    if(dx > 0) //direction right
    {
        if (rightA > leftB && leftA < leftB && bottomA > topB && topA < bottomB) 
        {
            collision = rightCollision;
            rightCol = true;
        }
    }
    if(dx < 0) //direction left
    {
        if (leftA < rightB && rightA > rightB && bottomA > topB && topA < bottomB) 
        {
            collision = leftCollision;
            leftCol = true;
        }
    }
    if(dy > 0) //direction bot
    {
        if (bottomA > topB && topA < topB && rightA > leftB && leftA < rightB) 
        {
            collision = botCollision;
            botCol = true;
        }
    }
    if(dy < 0) //direction top
    {
        if (topA < bottomB && bottomA > bottomB && rightA > leftB && leftA < rightB) 
        {
            collision = topCollision;
            topCol = true;
        }
    }

    //this is most likely useless
    //current problem: when we hold right/left wall and go up we are stuck but if we go down we're fine dunno why
    if(botCol == true && rightCol == true) //direction bot right
    {
        collision = botrightCollision;
    }
    if(topCol == true && rightCol == true) //direction top right
    {
        collision = toprightCollision;
    }
    if(botCol == true && leftCol == true) //direction bot left
    {
        collision = botleftCollision;
    }
    if(topCol == true && leftCol == true) //direction top left
    {
        collision = topleftCollision;
    }
    
    return collision;
}

void checkCollisionWall(Player* player, GridMap* map)
{
    int dx = player->playerVelocityX / 60;
    int dy = player->playerVelocityY / 60;

    for (int y = 0; y < GRID_HEIGHT; y++) 
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            if (strcmp(map->cells[y][x].tag, "Wall") == 0)
            {
                Collision collision = checkCollision(player, map->cells[y][x].cellRect);
                if(collision != noCollision)
                {
                    switch(collision)
                    {
                        case topCollision:
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case botCollision:
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case leftCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            break;
                        case rightCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            break;
                        case toprightCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case topleftCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case botrightCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case botleftCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        default:
                            printf("Error getting collision direction\n");
                            break;
                    }
                }
            }
        }
    }
} 