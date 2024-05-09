#include "player.h"
#include <SDL2/SDL_image.h>

#define SPEED 300
#define PLAYER_FRAME_RATE 60
#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

Player* createPlayer(SDL_Renderer* renderer) {
    Player* pPlayer = malloc(sizeof(Player));
    if (!pPlayer) {
        return NULL;
    }

    pPlayer->playerSurface = IMG_Load("resources/player1.png");
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
    pPlayer->playerX = pPlayer->playerRect.w;
    pPlayer->playerY = pPlayer->playerRect.h;
    pPlayer->playerVelocityX = 0;
    pPlayer->playerVelocityY = 0;
    pPlayer->speed = SPEED;
    pPlayer->camera.x = 0;
    pPlayer->camera.y = 0;
    pPlayer->camera.h = WINDOW_HEIGHT;
    pPlayer->camera.w = WINDOW_WIDTH; 

    return pPlayer;
}

void handlePlayerInput(Player* player, int up, int down, int left, int right, int levelWidth, int levelHeight) 
{
    player->playerVelocityX = player->playerVelocityY = 0;
    if (up && !down) player->playerVelocityY = -(player->speed);
    if (down && !up) player->playerVelocityY = player->speed;
    if (left && !right) player->playerVelocityX = -(player->speed);
    if (right && !left) player->playerVelocityX = player->speed;
    player->playerX += player->playerVelocityX / 60;
    player->playerY += player->playerVelocityY / 60;

    if ((player->playerX < 0) || (player->playerX + player->playerRect.w > levelWidth)) 
    {
        printf("PX back\n");
        player->playerX -= (player->playerVelocityX / 60);
    }
    if ((player->playerY < 0) || (player->playerY + player->playerRect.h > levelHeight))
    {
        printf("PY back\n");
        player->playerY -= (player->playerVelocityY / 60);
    }
    // player->playerX -= player->camera.x;
    // player->playerY -= player->camera.y; 
    player->playerRect.x = player->playerX - player->camera.x;
    player->playerRect.y = player->playerY - player->camera.y; 
    //printf("PX: %d, PY %d, levelh: %d, levelw: %d\n", (player->playerX + player->playerRect.w/2), (player->playerY + player->playerRect.h/2), levelHeight, levelWidth);
}

void renderPlayer(Player* player, SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, player->pPlayerTexture, NULL, &player->playerRect);
}

void destroyPlayer(Player* player) {
    SDL_DestroyTexture(player->pPlayerTexture);
    free(player);
}

