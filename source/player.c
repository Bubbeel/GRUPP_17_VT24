#include "player.h"
#include <SDL2/SDL_image.h>

Player* createPlayer(SDL_Renderer* renderer, int speed) {
    Player* pPlayer = malloc(sizeof(Player));
    if (!pPlayer) {
        return NULL;
    }

    SDL_Surface* pPlayerSurface = IMG_Load("resources/player1.png");
    if (!pPlayerSurface) {
        free(pPlayer);
        return NULL;
    }

    pPlayer->pPlayerTexture = SDL_CreateTextureFromSurface(renderer, pPlayerSurface);
    SDL_FreeSurface(pPlayerSurface);
    if (!pPlayer->pPlayerTexture) {
        free(pPlayer);
        return NULL;
    }

    SDL_QueryTexture(pPlayer->pPlayerTexture, NULL, NULL, &pPlayer->playerRect.w, &pPlayer->playerRect.h);

    pPlayer->playerRect.w /= 20;
    pPlayer->playerRect.h /= 20;
    pPlayer->playerX = pPlayer->playerRect.w;
    pPlayer->playerY = pPlayer->playerRect.h;
    pPlayer->playervelocityX = 0;
    pPlayer->playervelocityY = 0;
    pPlayer->speed = speed;

    return pPlayer;
}

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

void renderPlayer(Player* player, SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, player->pPlayerTexture, NULL, &player->playerRect);
}

void destroyPlayer(Player* player) {
    SDL_DestroyTexture(player->pPlayerTexture);
    free(player);
}

