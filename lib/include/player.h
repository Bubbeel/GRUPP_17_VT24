#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "music.h"
#include "playerData.h"
#include "gridmap.h"
#include "bullet.h"
#include "weapon.h"

typedef struct {
    SDL_Surface* playerSurface;
    SDL_Texture* pPlayerTexture;
    SDL_Rect playerRect;
    SDL_Rect camera;
    SDL_Rect srcRect;
    int playerNumber;
    int playerX, playerY;
    int playerVelocityX, playerVelocityY;
    int playerGridX, playerGridY; //used for getPlayerPosition and is saved here!!
    int speed;
    int alive;
    int health;
    int angle;
    int id;
    Bullet* pBullet;
    int weaponType;
    int lastDirection;
    int xStart, yStart;
    int playerPoints;
    int playerFrames;
    bool holdingFlag;
    bool holdingFlagSound;
    bool shielded;
    bool frozen;
    bool slowed;
    Uint32 freezeEndTime;
    Uint32 slowEndTime;
} Player;

struct GridMap;

Player* createPlayer(SDL_Renderer* renderer, int startPosX, int startPoxY, int id);
void handlePlayerInput(Player* player, ClientData cData, int levelWidth, int levelHeight);
void renderPlayer(Player* player, SDL_Renderer* renderer, Player* playerCurr);
void getPlayerSendData(Player* player, PlayerData* playerData);
void updatePlayerWithRecievedData(Player* player, PlayerData* playerData);
void destroyPlayer(Player* player);


//Bullet and fire
void updatePlayer(Player* player);
void playerFireBullet(Player* player, int bulletType, SDL_Rect camera);
int hitPlayer(Player* pShooter, Player* pTarget, Weapon* weapon[], SDL_Rect camera);
float distance(int x1, int y1, int x2, int y2);
void applyEffectToTarget(Player* pTarget, Weapon* pWeapon, int weaponType);

//functions for weapon pick up items
void applyWeaponToPlayer(Player* player, Weapon* weapon, SDL_Rect camera);
int weaponCollision(SDL_Rect weapon, SDL_Rect player);

//gridmap functions with Player and player camera
void renderVisibleMap(SDL_Renderer *renderer, GridMap *map, Player* player, int screenWidth, int screenHeight);
void renderGridMapCentered(SDL_Renderer *renderer, GridMap* map, Player* player, SDL_Texture* GridSpriteTexture, int windowWidth, int windowHeight, int levelWidth, int levelHeight);

//rendering players
void resetClients(Player* player);

#endif /* PLAYER_H */