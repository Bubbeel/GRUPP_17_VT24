#include "player.h"
#include "playerData.h"
#include <SDL2/SDL_image.h>

#define SPEED 300
#define PLAYER_FRAME_RATE 60
#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

Player* createPlayer(SDL_Renderer* renderer, int startPosX, int startPoxY) {
    Player* pPlayer = malloc(sizeof(Player));
    if (!pPlayer) {
        return NULL;
    }

    pPlayer->playerSurface = IMG_Load("../lib/resources/player1.png");
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
    pPlayer->playerY = startPoxY;
    pPlayer->playerRect.x = pPlayer->playerX;
    pPlayer->playerRect.y = pPlayer->playerY;
    pPlayer->playerVelocityX = 0;
    pPlayer->playerVelocityY = 0;
    pPlayer->speed = SPEED;
    pPlayer->alive = 1;
    pPlayer->camera.x = 0;
    pPlayer->camera.y = 0;
    pPlayer->camera.h = WINDOW_HEIGHT;
    pPlayer->camera.w = WINDOW_WIDTH; 

    return pPlayer;
}

void handlePlayerInput(Player* player, ClientData cData, int levelWidth, int levelHeight) 
{
    player->playerVelocityX = player->playerVelocityY = 0;
    if (cData.command.UP && !cData.command.DOWN) player->playerVelocityY = -(player->speed);
    if (cData.command.DOWN && !cData.command.UP) player->playerVelocityY = player->speed;
    if (cData.command.LEFT && !cData.command.RIGHT) player->playerVelocityX = -(player->speed);
    if (cData.command.RIGHT && !cData.command.LEFT) player->playerVelocityX = player->speed;
    player->playerX += player->playerVelocityX / 60;
    player->playerY += player->playerVelocityY / 60;

    if ((player->playerX < 0) || (player->playerX + player->playerRect.w > levelWidth)) 
    {
        player->playerX -= (player->playerVelocityX / 60);
    }
    if ((player->playerY < 0) || (player->playerY + player->playerRect.h > levelHeight))
    {
        player->playerY -= (player->playerVelocityY / 60);
    }
    player->playerRect.x = player->playerX - player->camera.x;
    player->playerRect.y = player->playerY - player->camera.y; 
}

void renderPlayer(Player* player, SDL_Renderer* renderer, Player* playerCurr) {
    player->playerRect.x = player->playerX - playerCurr->camera.x;
    player->playerRect.y = player->playerY - playerCurr->camera.y;
    SDL_RenderCopy(renderer, player->pPlayerTexture, NULL, &player->playerRect);
    // if(!player->alive)
    // {
    //     return;
    // }
}

void getPlayerSendData(Player* player, PlayerData* playerData)
{
    playerData->alive = player->alive;
    playerData->vx = player->playerVelocityX;
    playerData->vy = player->playerVelocityY;
    playerData->x = player->playerX;
    playerData->y = player->playerY;
    //getBulletSendData(pRocket->pBullet,&(pRocketData->bData)); add this when bullet gets added to player
}

void updatePlayerWithRecievedData(Player* player, PlayerData* playerData)
{
    player->alive = playerData->alive;
    player->playerVelocityX = playerData->vx;
    player->playerVelocityY = playerData->vx;
    player->playerX = playerData->x;
    player->playerY = playerData->y;
    //printf("plaryer nr: %d, player alive %d, player vx: %d, player vy: %d, playerx: %d, playery: %d\n", player->playerNumber, player->alive, player->playerVelocityX, player->playerVelocityY, player->playerX, player->playerY);
    //updateBulletWithRecievedData(pRocket->pBullet,&(pRocketData->bData)); add this when bullet gets added to player
}

void destroyPlayer(Player* player) {
    SDL_DestroyTexture(player->pPlayerTexture);
    free(player);
}
