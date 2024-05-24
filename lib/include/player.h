#ifndef PLAYER_H
#define PLAYER_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "playerData.h"
#include "flag.h"
typedef struct {
    SDL_Surface* playerSurface;
    SDL_Texture* pPlayerTexture;
    SDL_Rect playerRect;
    SDL_Rect camera;
    int playerNumber;
    int playerX, playerY;
    int playerVelocityX, playerVelocityY;
    int playerGridX, playerGridY; //used for getPlayerPosition and is saved here!!
    int speed;
    int alive;
} Player;

struct GridMap;

Player* createPlayer(SDL_Renderer* renderer, int startPosX, int startPoxY);
void handlePlayerInput(Player* player, ClientData cData, int levelWidth, int levelHeight);
void renderPlayer(Player* player, SDL_Renderer* renderer, Player* playerCurr);
void getPlayerSendData(Player* player, PlayerData* playerData, Flag* flag);
void updatePlayerWithRecievedData(Player* player, PlayerData* playerData, Flag* flag);
void destroyPlayer(Player* player);

#endif /* PLAYER_H */