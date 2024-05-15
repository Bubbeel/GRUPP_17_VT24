#ifndef collisionDetection_h
#define collisionDetection_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "../objects/gridmap.h"
#include "../objects/player.h"

typedef enum
{
    noCollision,
    topCollision,
    botCollision,
    leftCollision,
    rightCollision,
    topleftCollision,
    toprightCollision,
    botleftCollision,
    botrightCollision
}Collision;

Collision checkCollision(Player* player, SDL_Rect b);
void checkCollisionWall(Player* player, GridMap* map);

#endif //collisionDetection_h