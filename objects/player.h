#ifndef PLAYER_H
#define PLAYER_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#pragma pack(push, 1)
typedef struct {
    SDL_Surface* playerSurface;
    SDL_Texture* pPlayerTexture;
    SDL_Rect playerRect;
    SDL_Rect camera;
    int playerX, playerY;
    int playerVelocityX, playerVelocityY;
    int playerGridX, playerGridY; //used for getPlayerPosition and is saved here!!
    int speed;
} Player;
#pragma pack(pop)

struct GridMap;

Player* createPlayer(SDL_Renderer* renderer, int startPosX, int startPoxY);
void handlePlayerInput(Player* player, int up, int down, int left, int right, int levelWidth, int levelHeight);
void renderPlayer(Player* player, SDL_Renderer* renderer);
void destroyPlayer(Player* player);

#endif /* PLAYER_H */