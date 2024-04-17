#include "player.h"

void handlePlayerInput(SDL_Rect *playerRect, float *playerX, float *playerY, float *playerVelocityX, float *playerVelocityY, int up, int down, int left, int right, int windowWidth, int windowHeight, int playerRectWidth, int playerRectHeight, int speed) {
    *playerVelocityX = *playerVelocityY = 0;
    if (up && !down) *playerVelocityY = -speed;
    if (down && !up) *playerVelocityY = speed;
    if (left && !right) *playerVelocityX = -speed;
    if (right && !left) *playerVelocityX = speed;
    *playerX += *playerVelocityX / 60;
    *playerY += *playerVelocityY / 60;
    if (*playerX < 0) *playerX = 0;
    if (*playerY < 0) *playerY = 0;
    if (*playerX > windowWidth - playerRectWidth) *playerX = windowWidth - playerRectWidth;
    if (*playerY > windowHeight - playerRectHeight) *playerY = windowHeight - playerRectHeight;
    playerRect->x = *playerX;
    playerRect->y = *playerY;
}
