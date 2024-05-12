#ifndef collisionDetection_h
#define collisionDetection_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "gridMap.h"
#include "player.h"

bool checkCollision(SDL_Rect a, SDL_Rect b);
void checkCollisionWall(Player* player, GridMap* map);

#endif //collisionDetection_h