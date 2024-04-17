#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

void handlePlayerInput(SDL_Rect *playerRect, float *playerX, float *playerY, float *playerVelocityX, float *playerVelocityY, int up, int down, int left, int right, int windowWidth, int windowHeight, int playerRectWidth, int playerRectHeight, int speed);

#endif /* PLAYER_H */