#ifndef WEAPONS_H
#define WEAPONS_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "gridMap.h"


// #define MAX_PLAYERS 4
#define perk_duration 5
#define DEFAULT_SPEED 2
#define WEAPON_WIDTH 32
#define WEAPON_HEIGHT 32
#define SPEED_BOOST_AMOUNT 1

#define PLAYER_SPEED 100
#include "playerData.h"



typedef struct {
    SDL_Surface* weapSurface;
    SDL_Texture *texture;
    SDL_Rect rect;
    bool available;
    int duration;
    int weaponId;
    int posX,posY;
    int bulletType;
} Weapon;

Weapon* createWeapon(SDL_Renderer* renderer, int weaponId, GridMap* map); 
void renderWeapon(SDL_Renderer *renderer, Weapon* weapon, SDL_Rect camera);
void destroyWeapon(Weapon* weapon);
void destroyWeapons(Weapon* weapons[]);

#endif