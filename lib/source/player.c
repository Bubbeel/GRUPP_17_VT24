#include "player.h"
#include "playerData.h"
#include <SDL2/SDL_image.h>

#define M_PI 3.14159265358979323846
#define SPEED 500
#define PLAYER_FRAME_RATE 60
#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

Player* createPlayer(SDL_Renderer* renderer, int startPosX, int startPoxY, int id) {
    Player* pPlayer = malloc(sizeof(Player));
    if (!pPlayer) {
        return NULL;
    }

    pPlayer->pBullet = createBullet(renderer, 2816, 2100);
    if (!pPlayer->pBullet) {
        free(pPlayer);
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

    pPlayer->playerRect.w /= 25;
    pPlayer->playerRect.h /= 25;
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

    pPlayer->health = 40;
    pPlayer->angle = 0;
    pPlayer->id = id;
    pPlayer->weaponType = 0;
    pPlayer->lastDirection = 90;
    pPlayer->xStart = pPlayer->playerX;
    pPlayer->yStart = pPlayer->playerY;
    pPlayer->playerPoints = 0;
    pPlayer->playerFrames = 0;

    pPlayer->frozen = false;
    pPlayer->shielded = false;
    pPlayer->slowed = false;
    pPlayer->holdingFlag = false;
    pPlayer->holdingFlagSound = false;


    return pPlayer;
}

void handlePlayerInput(Player* player, ClientData cData, int levelWidth, int levelHeight) 
{
    static bool isWalkingSoundPlaying = false;
    player->playerVelocityX = player->playerVelocityY = 0;
    if (cData.command.UP && !cData.command.DOWN){
        player->playerVelocityY = -(player->speed);
        player->lastDirection = 0; // up
    }else if (cData.command.DOWN && !cData.command.UP){
        player->playerVelocityY = player->speed;
        player->lastDirection = 180; // down
    }else if (cData.command.LEFT && !cData.command.RIGHT){
        player->playerVelocityX = -(player->speed);
        player->lastDirection = 270; // left
    }else if (cData.command.RIGHT && !cData.command.LEFT){
        player->playerVelocityX = player->speed;
        player->lastDirection = 90; // right
    }else if (cData.command.FIRE){
        printf("Shooting works1\n");
        playerFireBullet(player,1, player->camera);
        printf("Shooting works2\n");
    }

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

    if(!player->alive)
    {
        playPlayerDiedSound();
        resetClients(player);
        return;
    }

    player->playerRect.x = player->playerX - playerCurr->camera.x;
    player->playerRect.y = player->playerY - playerCurr->camera.y;
    SDL_RenderCopy(renderer, player->pPlayerTexture, NULL, &player->playerRect);

}



void resetClients(Player* player){

    player->playerRect.x=player->playerX=player->xStart;
    player->playerRect.y=player->playerY=player->yStart;
    player->angle = 0;
    player->playerVelocityX=player->playerVelocityY=0;
    player->alive = 1;
    player->speed = SPEED;

    player->health = 40;
    player->frozen = false;
    player->shielded = false;
    player->slowed = false;

    player->weaponType = 0;
    player->lastDirection = 90;
    player->holdingFlag = false;
    player->playerPoints = 0;

    if(aliveBullet(player->pBullet)){
        killBullet(player->pBullet);
    }
}

void getPlayerSendData(Player* player, PlayerData* playerData)
{
    playerData->alive = player->alive;
    playerData->vx = player->playerVelocityX;
    playerData->vy = player->playerVelocityY;
    playerData->x = player->playerX;
    playerData->y = player->playerY;

    getBulletSendData(player->pBullet,&(playerData->bData)); //add this when bullet gets added to player
}

void updatePlayerWithRecievedData(Player* player, PlayerData* playerData)
{
    player->alive = playerData->alive;
    player->playerVelocityX = playerData->vx;
    player->playerVelocityY = playerData->vx;
    player->playerX = playerData->x;
    player->playerY = playerData->y;
    
    updateBulletWithRecievedData(player->pBullet,&(playerData->bData)); //add this when bullet gets added to player
}

void destroyPlayer(Player* player) {
    SDL_DestroyTexture(player->pPlayerTexture);
    free(player);
}























void updatePlayer(Player* player){ // ska ha med i player.c
    //if(!pRocket->alive) return;
/*
    if (player->slowed && SDL_GetTicks() >= player->slowEndTime) {
        // Unslow the player
        printf("unslow player\n");
        player->slowed = false;
        player->speed = SPEED; // Restore original speed
    }
    if (player->frozen && SDL_GetTicks() >= player->freezeEndTime) {
        // Unfreeze the player
        printf("unfreeze player\n");
        player->frozen = false;
        player->speed = SPEED; // Restore original speed
    }
*/


    /*
    player->playerX+=player->playervelocityX;
    player->playerY+=player->playervelocityY;
    if(player->playerX<0) player->playerX+=player->window_width;
    else if (player->playerX>player->window_width) player->playerX-=player->window_width;
    if(player->playerY<0) player->playerY+=player->window_height;
    else if(player->playerY>player->window_height) player->playerY-=player->window_height;
    */
    //player->playerRect.x=player->playerX;
    //player->playerRect.y=player->playerY;
    if(!aliveBullet(player->pBullet)) return;
    updateBullet(player->pBullet);
}






// player fires bullet
void playerFireBullet(Player* player, int bulletType, SDL_Rect camera){
    if(!player->alive || aliveBullet(player->pBullet)) return;

    int angle = player->lastDirection;
    printf("Angle = %d\n", angle);
    float bulletvx = 12*sin(angle*2*M_PI/360); // you change bullet speed here for x
    float bulletvy = -12*cos(angle*2*M_PI/360); // you change bullet speed here for y

    player->pBullet->type = bulletType;

    printf("Bullet velocity: %d\n", bulletvx);
    printf("Bullet velocity: %d\n", bulletvy);

    startBullet(player->pBullet,
                player->playerX+player->playerRect.w/2,
                player->playerY+player->playerRect.h/2,
                bulletvx,
                bulletvy,
                camera);

                //player->playerVelocityX+
                //player->playerVelocityY+
}




// bullet collision with target
int hitPlayer(Player* pShooter, Player* pTarget, Weapon* weapon[], SDL_Rect camera){ // kvar att implementera i nya
    if(!pShooter->alive || !pTarget->alive || !aliveBullet(pShooter->pBullet) || pShooter->id == pTarget->id) return 0;
    //printf("Inside hitPlayer function1\n");


    int collide = distance(xBullet(pShooter->pBullet),yBullet(pShooter->pBullet),
                                    pTarget->playerRect.x+pTarget->playerRect.w/2,
                                    pTarget->playerRect.y+pTarget->playerRect.h/2)<(pTarget->playerRect.w)/2;
    
    SDL_Rect pTargetRect = {
        pTarget->playerRect.x = pTarget->playerX - camera.x,
        pTarget->playerRect.x = pTarget->playerY - camera.y,
        pTarget->playerRect.w,
        pTarget->playerRect.h,
    };

    SDL_Rect pShooterRect = {
        pShooter->pBullet->rect.x = pShooter->pBullet->x - camera.x,
        pShooter->pBullet->rect.y = pShooter->pBullet->y - camera.y,
        pShooter->pBullet->rect.w,
        pShooter->pBullet->rect.h,
    };
    
    //printf("Inside hitPlayer function2\n");
    if(collide || SDL_HasIntersection(&pShooterRect, &pTargetRect)){
        playHurtSound();
        printf("Inside hitPlayer function3\n");
        if(pShooter->pBullet->type>0 && pShooter->weaponType>0 && pShooter->pBullet->type<4 && pShooter->weaponType <4){
            printf("Inside hitPlayer function4\n");
            for (int i = 0; weapon[i] != NULL; i++){ // weapon[i]->weaponId != 0 && weapon[i]->weaponId < 4
                printf("Inside hitPlayer function5\n");
                if(weapon[i]->available && weapon[i]->weaponId == pShooter->weaponType){
                    printf("Inside hitPlayer function6\n");
                    applyEffectToTarget(pTarget, weapon[i], pShooter->weaponType);
                    pShooter->pBullet->type = 0;
                    pShooter->weaponType = 0;
                    printf("Inside hitPlayer function7\n");
                    break;
                }
            }
        }else{ // default damage
            printf("Inside hitPlayer function8\n");
            printf("Target before health: %d\n", pTarget->health);
            pTarget->health -= 10;
            printf("Target health: %d\n", pTarget->health);
        }
        printf("Inside hitPlayer function9\n");
        if(pTarget->health == 0 || pTarget->health <= 0){
            printf("Target dead: %d\n", pTarget->health);
            pTarget->alive = 0;
        }
        printf("Inside hitPlayer function10\n");
        killBullet(pShooter->pBullet);
        printf("Inside hitPlayer function11\n");
    }
    return collide;
}


// functions in hitPlayer

float distance(int x1, int y1, int x2, int y2){
    return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}






void applyEffectToTarget(Player* pTarget, Weapon* pWeapon, int weaponType) { // applies weapon effects on targets hit by bullet
    if(weaponType > 0 && weaponType < 4){
        switch (weaponType) {
            case 1: // slow stave
                // Apply slow down effect to player
                printf("Target before speed: %d\n", pTarget->speed);
                pTarget->slowed = true;
                pTarget->speed /= 2; // Reduce player speed by half
                pTarget->slowEndTime = SDL_GetTicks() + 6000;
                pWeapon->available = false;
                pTarget->health -= 10;
                printf("Target health: %d\n", pTarget->health);
                printf("Target after speed: %d\n", pTarget->speed);
                
                break;
            case 2: // freeze stave
                // Apply freeze effect to player
                pTarget->frozen = true; // Set player frozen state to true
                pTarget->speed = 0;
                pTarget->freezeEndTime = SDL_GetTicks() + 3000;
                pWeapon->available = false;
                pTarget->health -= 10;
                break;
            case 3: // damage flame stave
                // Apply damage effect to player
                pWeapon->available = false;
                pTarget->health -= 20; // Reduce player health by 20
                //void playHurtSound();
                break;
            case 4:
                // shield
                break;
            default:
                // Unknown weapon ID
                break;
        }
    }
}






// weapon and player collisions

void applyWeaponToPlayer(Player* player, Weapon* weapon, SDL_Rect camera) { // checks collision between player and with an item on the map
    
    // checking with world position, not render position
    SDL_Rect playerWorldRect = {
        player->playerX,
        player->playerY,
        player->playerRect.w,
        player->playerRect.h
    };

    // Define weapon world coordinates
    SDL_Rect weaponWorldRect = {
        weapon->posX,
        weapon->posY,
        weapon->rect.w,
        weapon->rect.h
    };

    if(weaponCollision(playerWorldRect, weaponWorldRect)){
        playPlayerPickUpItemSound();
                //player->playerRect, weapon->rect
        switch (weapon->weaponId) {
            case 1: // slow stave
                // Apply slow down effect to player
                player->pBullet->type = 1;
                printf("player id: %d\n", player->id);
                printf("bullet type set to 1\n");
                player->weaponType = 1;
                printf("weaponType to 1\n");
                weapon->available = false;
                printf("weaponAvailable to true\n");
                //player->pBullet->textureId = 1;
                //printf("Bullet texture id: 1\n");
                weapon->posX=weapon->posY=weapon->rect.x=weapon->rect.y = 0;
                break;
            case 2: // freeze stave
                // Apply freeze effect to player
                printf("player id: %d\n", player->id);
                player->pBullet->type = 2; // Set player frozen state to true
                printf("bullet type set to 2\n");
                player->weaponType = 2;
                printf("weaponType to 2\n");
                weapon->available = false;
                printf("weaponAvailable to true\n");
                //player->pBullet->textureId = 2;
                //printf("Bullet texture id: 2\n");
                weapon->posX=weapon->posY=weapon->rect.x=weapon->rect.y = 0;
                break;
            case 3: // damage stave
                printf("player id: %d\n", player->id);
                // Apply damage effect to player
                player->pBullet->type = 3; // Reduce player health by 10
                printf("bullet type set to 3\n");
                player->weaponType = 3;
                printf("weaponType to 3\n");
                weapon->available = false;
                printf("weaponAvailable to true\n");
                //player->pBullet->textureId = 3;
                //printf("Bullet texture id: 3\n");
                weapon->posX=weapon->posY=weapon->rect.x=weapon->rect.y = 0;
                break;
            case 4: // protection shield
                printf("player id: %d\n", player->id);
                // Apply protection shield effect to player
                player->shielded = false; // Set player shielded state to true
                printf("shielded is true\n");
                player->weaponType = 4;
                printf("weaponType to 4\n");                
                player->health += 20;
                printf("Current heath: %d\n", player->health);
                weapon->posX=weapon->posY=weapon->rect.x=weapon->rect.y = 0;
                break;
            default:
                // Unknown weapon ID
                break;
        }
    }
}




// weapon and player collision
int weaponCollision(SDL_Rect weapon, SDL_Rect player) // check weapon/player collision
{
    if((weapon.x + weapon.w > player.x && weapon.x < player.x + player.w) && 
        (weapon.y + weapon.h > player.y && weapon.y < player.y + player.h)){
        printf("Weapon changed coordinates for Rectx: %d\n", weapon.x);
        printf("Weapon changed coordinates for Recty: %d\n", weapon.y);
        return 1;
    }

    return 0;
}















//gridmap and Player camera

// New function to render grid map centered around player
void renderGridMapCentered(SDL_Renderer *renderer, GridMap* map, Player* player, SDL_Texture* GridSpriteTexture, int windowWidth, int windowHeight, int levelWidth, int levelHeight) {
    // Calculate the top-left corner of the screen based on player's position
    player->camera.x = (player->playerX + player->playerRect.w/2) - windowWidth / 2;
    player->camera.y = (player->playerY + player->playerRect.h/2) - windowHeight / 2;


    // Ensure screen bounds are within the world bounds
    if (player->camera.x < 0) {
        player->camera.x = 0;
    } 
    else if (player->camera.x + windowWidth > levelWidth) 
    {
        player->camera.x = levelWidth - windowWidth;
    }

    if (player->camera.y < 0) {
        player->camera.y = 0;
    } 
    else if (player->camera.y + windowHeight > levelHeight) 
    {
        player->camera.y = levelHeight - windowHeight;
    }

    // Render the grid map relative to the calculated screen position
    renderGridMap(renderer, map, GridSpriteTexture, player->camera.x, player->camera.y, windowWidth, windowHeight);
}


void renderVisibleMap(SDL_Renderer *renderer, GridMap *map, Player* player, int screenWidth, int screenHeight) 
{
    // Calculate the top-left corner of the visible area based on the camera's position
    int cameraX = player->playerX - (screenWidth /2); //remove /2 if error fixing, not sure what it does
    int cameraY = player->playerY - (screenHeight /2);

    // Ensure the camera stays within the bounds of the map
    if (cameraX < 0) 
    {
        cameraX = 0;
    }
    if (cameraY < 0) 
    {
        cameraY = 0;
    }
    if (cameraX > GRID_WIDTH*CELL_SIZE - screenWidth) 
    {
        cameraX = GRID_WIDTH*CELL_SIZE - screenWidth;
    }
    if (cameraY > GRID_HEIGHT*CELL_SIZE - screenHeight) 
    {
        cameraY = GRID_HEIGHT*CELL_SIZE - screenHeight;
    }

    // Calculate the position of the camera relative to the player
    int offsetX = player->playerX - cameraX;
    int offsetY = player->playerY - cameraY;
    printf("OffsetX: %d, OffsetY: %d \n", offsetX, offsetY);


    // Render only the portion of the map that is visible on the screen
    for (int y = cameraY; y < screenHeight/CELL_SIZE; y++) 
    {
        for (int x = cameraX; x < screenWidth/CELL_SIZE; x++) 
        {
            // Calculate the position of the cell on the screen
            int mapX = cameraX + x;
            int mapY = cameraY + y;
            if (mapX >= 0 && mapX < GRID_WIDTH && mapY >= 0 && mapY < GRID_HEIGHT) 
            {
                // Calculate the screen position of the cell
                int screenX = x * CELL_SIZE - offsetX;
                int screenY = y * CELL_SIZE - offsetY;

                // Render the cell at the calculated screen position
                SDL_Rect cellRect = {screenX, screenY, CELL_SIZE, CELL_SIZE};
                switch (map->cells[mapY][mapX].type) 
                {
                    case EMPTY:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Render empty cell as white
                        break;
                    case OBSTACLE:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Render obstacle cell as black
                        break;
                    case FLAG:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 0 ,255); // Render flag cell as yellow
                        break;
                    default:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Default to white for unknown cell types
                        break;
                }
                SDL_RenderFillRect(renderer, &cellRect); // Render the cell
            }  
        }
    }
}




