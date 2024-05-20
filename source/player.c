#include "../objects/player.h"
#include <SDL2/SDL_image.h>

#define SPEED 300
#define PLAYER_FRAME_RATE 60
#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800
#define PLAYER_SPEED 10



Player* createPlayer(SDL_Renderer* renderer, int startPosX, int startPosY) {
    Player* pPlayer = malloc(sizeof(Player));
    if (!pPlayer) {
        return NULL;
    }

    pPlayer->playerSurface = IMG_Load("resources/player1.png"); // Default texture
    if (!pPlayer->playerSurface) {
        free(pPlayer);
        return NULL;
    }

    pPlayer->pPlayerTexture = SDL_CreateTextureFromSurface(renderer, pPlayer->playerSurface);
    SDL_FreeSurface(pPlayer->playerSurface);
    if (!pPlayer->pPlayerTexture) {
        free(pPlayer);
        return NULL;
    }

    SDL_QueryTexture(pPlayer->pPlayerTexture, NULL, NULL, &pPlayer->playerRect.w, &pPlayer->playerRect.h);
    pPlayer->playerRect.w /= 20;
    pPlayer->playerRect.h /= 20;
    pPlayer->playerX = startPosX;
    pPlayer->playerY = startPosY;
    pPlayer->playerVelocityX = 0;
    pPlayer->playerVelocityY = 0;
    pPlayer->speed = PLAYER_SPEED;
    pPlayer->camera.x = 0;
    pPlayer->camera.y = 0;
    pPlayer->camera.h = WINDOW_HEIGHT;
    pPlayer->camera.w = WINDOW_WIDTH;
    pPlayer->playerRect.x = pPlayer->playerX;
    pPlayer->playerRect.y = pPlayer->playerY;

    printf("Player created at X=%d, Y=%d\n", pPlayer->playerX, pPlayer->playerY);
    return pPlayer;
}



void handlePlayerInput(Player* player, int up, int down, int left, int right, int levelWidth, int levelHeight) {
    player->playerVelocityX = player->playerVelocityY = 0;

    if (up && !down) {
        player->playerVelocityY = -player->speed;
    }
    if (down && !up) {
        player->playerVelocityY = player->speed;
    }
    if (left && !right) {
        player->playerVelocityX = -player->speed;
    }
    if (right && !left){
        player->playerVelocityX = player->speed;
    } 

    player->playerX += player->playerVelocityX;
    player->playerY += player->playerVelocityY;

    if (player->playerX + player->playerRect.w > levelWidth) player->playerX = levelWidth - player->playerRect.w;
    if (player->playerY + player->playerRect.h > levelHeight) player->playerY = levelHeight - player->playerRect.h;

    player->playerRect.x = (int)(player->playerX - player->camera.x);
    player->playerRect.y = (int)(player->playerY - player->camera.y);

    // Debug output to verify playerRect updates correctly
    printf("Player input handled: playerX=%d, playerY=%d, playerRect.x=%d, playerRect.y=%d, velocityX=%d, velocityY=%d\n",
           player->playerX, player->playerY, player->playerRect.x, player->playerRect.y, player->playerVelocityX, player->playerVelocityY);
}


void renderPlayer(Player* player, SDL_Renderer* renderer) {
    printf("Rendering player at playerRect.x=%d, playerRect.y=%d\n", player->playerRect.x, player->playerRect.y);
    SDL_RenderCopy(renderer, player->pPlayerTexture, NULL, &player->playerRect);
}

void destroyPlayer(Player* player) {
    SDL_DestroyTexture(player->pPlayerTexture);
    free(player);
}
