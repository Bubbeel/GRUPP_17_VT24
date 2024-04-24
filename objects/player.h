#ifndef PLAYER_H
#define PLAYER_H
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Texture* pPlayerTexture;
    SDL_Rect playerRect;
    float playerX, playerY;
    float playervelocityX, playervelocityY;
    int speed;
} Player;

Player* createPlayer(SDL_Renderer* renderer, int speed);
void handlePlayerInput(SDL_Rect *playerRect, float *playerX, float *playerY, float *playerVelocityX, float *playerVelocityY, int up, int down, int left, int right, int windowWidth, int windowHeight, int playerRectWidth, int playerRectHeight, int speed);
void renderPlayer(Player* player, SDL_Renderer* renderer);
void destroyPlayer(Player* player);

#endif /* PLAYER_H */