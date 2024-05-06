#ifndef PLAYER_H
#define PLAYER_H
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Surface* playerSurface;
    SDL_Texture* pPlayerTexture;
    SDL_Rect playerRect;
    int playerX, playerY;
    int playervelocityX, playervelocityY;
    int playerGridX, playerGridY; //used for getPlayerPosition and is saved here!!
    char direction;
    int speed;
} Player;

Player* createPlayer(SDL_Renderer* renderer, int speed);
void handlePlayerInput(SDL_Rect *playerRect, int *playerX, int *playerY, int *playerVelocityX, int *playerVelocityY, int up, int down, int left, int right, int windowWidth, int windowHeight, int playerRectWidth, int playerRectHeight, int speed);
void renderPlayer(Player* player, SDL_Renderer* renderer);
void destroyPlayer(Player* player);

#endif /* PLAYER_H */