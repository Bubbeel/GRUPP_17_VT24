#ifndef collisionDetection_h
#define collisionDetection_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "gridmap.h"
#include "player.h"
#include "flag.h"
#include "menu.h"
#include "music.h"

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

//collision check for bullet
Collision checkCollisionBullet(Bullet* bullet, SDL_Rect b);
void checkBulletCollisionWall(Player* player, GridMap* map);

//collision check for Flag
Collision checkCollisionFlag(Player* player, Flag* flag);
bool checkFlagCellCollision(Flag* flag, GridCell* cell);
bool handleFlagDelivery(Player* player, Flag* flag, GridMap* map, int goalX, int goalY);
int playerGotFlagCheck(Player* player, Flag* flag);
int playerDeliveredFlag(SDL_Renderer* pRenderer, Player* player, Flag* flag, GridMap* map, GameUI* currentPointsDisplayUI);
#endif //collisionDetection_h