#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "collisionDetection.h"



Collision checkCollision(Player* player, SDL_Rect b)
{
    // Calculate the player's movement direction

    int dx = player->playerVelocityX / 60;
    int dy = player->playerVelocityY / 60;

    int leftA, rightA, topA, bottomA;
    int leftB, rightB, topB, bottomB;

    Collision collision = noCollision;

    bool topCol = false;
    bool botCol = false;
    bool rightCol = false;
    bool leftCol = false;
    //Calculate the sides of rect A

    leftA = player->playerX;
    rightA = player->playerX + player->playerRect.w;
    topA = player->playerY;
    bottomA = player->playerY + player->playerRect.h;

    //Calculate the sides of rect B

    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    if(dx > 0) //direction right
    {
        if (rightA > leftB && leftA < leftB && bottomA > topB && topA < bottomB) 
        {
            collision = rightCollision;
            rightCol = true;
        }
    }
    if(dx < 0) //direction left
    {
        if (leftA < rightB && rightA > rightB && bottomA > topB && topA < bottomB) 
        {
            collision = leftCollision;
            leftCol = true;
        }
    }
    if(dy > 0) //direction bot
    {
        if (bottomA > topB && topA < topB && rightA > leftB && leftA < rightB) 
        {
            collision = botCollision;
            botCol = true;
        }
    }
    if(dy < 0) //direction top
    {
        if (topA < bottomB && bottomA > bottomB && rightA > leftB && leftA < rightB) 
        {
            collision = topCollision;
            topCol = true;
        }
    }

    if(botCol == true && rightCol == true) //direction bot right
    {
        collision = botrightCollision;
    }
    if(topCol == true && rightCol == true) //direction top right
    {
        collision = toprightCollision;
    }
    if(botCol == true && leftCol == true) //direction bot left
    {
        collision = botleftCollision;
    }
    if(topCol == true && leftCol == true) //direction top left
    {
        collision = topleftCollision;
    }
    
    return collision;
}

void checkCollisionWall(Player* player, GridMap* map)
{
    int dx = player->playerVelocityX / 60;
    int dy = player->playerVelocityY / 60;

    for (int y = 0; y < GRID_HEIGHT; y++) 
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            if (strcmp(map->cells[y][x].tag, "Wall") == 0 || strcmp(map->cells[y][x].tag, "Wall1") == 0 || 
                strcmp(map->cells[y][x].tag, "Wall2") == 0 || strcmp(map->cells[y][x].tag, "Wall3") == 0 || map->cells[y][x].type == WALL)
            {
                Collision collision = checkCollision(player, map->cells[y][x].cellRect);
                if(collision != noCollision)
                {
                    switch(collision)
                    {
                        case topCollision:
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case botCollision:
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case leftCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            break;
                        case rightCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            break;
                        case toprightCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case topleftCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case botrightCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        case botleftCollision:
                            player->playerX -= player->playerVelocityX / 60;
                            player->playerY -= player->playerVelocityY / 60;
                            break;
                        default:
                            printf("Error getting collision direction\n");
                            break;
                    }
                }
            }
        }
    }
} 






// collision checks for bullet
Collision checkCollisionBullet(Bullet* bullet, SDL_Rect b)
{
    int leftA = bullet->x;
    int rightA = bullet->x + 15;  // Assuming the bullet is 15x15 pixels
    int topA = bullet->y;
    int bottomA = bullet->y + 15; // Assuming the bullet is 15x15 pixels

    int leftB = b.x;
    int rightB = b.x + b.w;
    int topB = b.y;
    int bottomB = b.y + b.h;

    // Check for no collision
    if (bottomA <= topB || topA >= bottomB || rightA <= leftB || leftA >= rightB) {
        return noCollision;
    }

    // Determine collision side
    if (rightA > leftB && leftA < leftB) {
        return rightCollision;
    }
    if (leftA < rightB && rightA > rightB) {
        return leftCollision;
    }
    if (bottomA > topB && topA < topB) {
        return botCollision;
    }
    if (topA < bottomB && bottomA > bottomB) {
        return topCollision;
    }

    return noCollision; // Fallback, should not reach here
}




void checkBulletCollisionWall(Player* player, GridMap* map)
{
    if (player->pBullet->time > 0) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (strcmp(map->cells[y][x].tag, "Wall") == 0 || map->cells[y][x].type == WALL){
                    Collision collision = checkCollisionBullet(player->pBullet, map->cells[y][x].cellRect);
                    if (collision != noCollision) {
                        killBullet(player->pBullet);
                        return; // Exit after detecting the first collision
                    }
                }
            }
        }
    }

} 




Collision checkCollisionFlag(Player* player, Flag* flag)
{
    flag->flagX = flag->flagRect.x;
    flag->flagY = flag->flagRect.y;
    // check with player world position not render position
    SDL_Rect playerWorldRect = {
        player->playerX,
        player->playerY,
        player->playerRect.w,
        player->playerRect.h
    };

    int leftA = player->playerX;
    int rightA = player->playerX + player->playerRect.w;  // Assuming the bullet is 32x32 pixels
    int topA = player->playerY;
    int bottomA = player->playerY + player->playerRect.h; // Assuming the bullet is 32x32 pixels

    int leftB = flag->flagX;
    int rightB = flag->flagX + flag->flagRect.w;
    int topB = flag->flagY;
    int bottomB = flag->flagY + flag->flagRect.h;

    // Check for no collision
    if (bottomA <= topB || topA >= bottomB || rightA <= leftB || leftA >= rightB) {
        return noCollision;
    }

    // Determine collision side
    if (rightA > leftB && leftA < leftB) {
        return rightCollision;
    }
    if (leftA < rightB && rightA > rightB) {
        return leftCollision;
    }
    if (bottomA > topB && topA < topB) {
        return botCollision;
    }
    if (topA < bottomB && bottomA > bottomB) {
        return topCollision;
    }

    return noCollision; // Fallback, should not reach here
}





bool checkFlagCellCollision(Flag* flag, GridCell* cell) {
    // Define the flag's rectangle
    SDL_Rect flagRect = {
        flag->flagX,
        flag->flagY,
        flag->flagRect.w,
        flag->flagRect.h
    };

    // Check if the flag's rectangle intersects with the cell's rectangle
    if (SDL_HasIntersection(&flagRect, &(cell->cellRect))) {
        printf("Hello it works flag and cell collision\n");
        return true;
    }
    return false;
}

// Function to handle flag delivery to a specific cell
bool handleFlagDelivery(Player* player, Flag* flag, GridMap* map, int goalX, int goalY) {

    GridCell* goalCell = &map->cells[goalY][goalX];

    if (checkFlagCellCollision(flag, goalCell)) {
        // Increase player's points
        printf("Players start position X and Y: %d, %d", player->xStart, player->yStart);
        player->playerPoints += 1;

        // Reset the flag position (example: to the center of the map)
        flag->flagX = flag->xStart;
        flag->flagY = flag->yStart;
        flag->flagRect.x = flag->xStart;
        flag->flagRect.y = flag->yStart;


        return true;
    }
    return false;
}


int playerGotFlagCheck(Player* player, Flag* flag){
   
    if (checkCollisionFlag(player, flag))
    {
        flag->flagRect.x = player->playerX+8;
        flag->flagRect.y = player->playerY-32;
        flag->flagX = player->playerX+8;
        flag->flagY = player->playerY-32;

        if(!player->holdingFlag){
            printf("Inside holdingFlag1\n");
            player->speed = player->speed*0.8;
            player->holdingFlag = true;
        }
        if(player->holdingFlag && !player->holdingFlagSound){
            printf("Inside holdingFlag2\n");
            playPlayerPickUpItemSound();
            playPlayerGotFlagSound();
            player->holdingFlagSound = true;
        }
    }else if(player->holdingFlag && player->speed < 500){
        printf("Inside holdingFlag3\n");
        player->speed = 500;
        player->holdingFlag = false;
        player->holdingFlagSound = false;
    }

    return 1;
}


int playerDeliveredFlag(SDL_Renderer* pRenderer, Player* player, Flag* flag, GridMap* map, GameUI* currentPointsDisplayUI){
    switch(player->id){
            case 0:
                if(handleFlagDelivery(player, flag, map, 5, 5) && playerGotFlagCheck(player, flag)){ // its possible that we need to add the pPlayer data member positions instead of 5,5
                    printf("Players start position X and Y 1: %d, %d", player->xStart, player->yStart);
                    printf("Player got point: %d\n", player->playerPoints);

                    if(!newDisplayUI(pRenderer, currentPointsDisplayUI, player->playerPoints)){
                        printf("Error with newDisplayUI 1: %s\n", SDL_GetError());
                    }
                    playPlayerGotPointSound();
                    if(player->playerPoints == 3){
                        playWinningSound();
                        if(!newDisplayUI(pRenderer, currentPointsDisplayUI, player->playerPoints)){
                            printf("Error with newDisplayUI 2: %s\n", SDL_GetError());
                        }
                    }
                }
                break;
            case 1:
                if(handleFlagDelivery(player, flag, map, 60, 5)){ // its possible that we need to add the pPlayer data member positions instead of 5,5
                    printf("Players start position X and Y 1: %d, %d", player->xStart, player->yStart);
                    printf("Player got point: %d\n", player->playerPoints);

                    if(!newDisplayUI(pRenderer, currentPointsDisplayUI, player->playerPoints)){
                        printf("Error with newDisplayUI 1: %s\n", SDL_GetError());
                    }
                    playPlayerGotPointSound();
                    if(player->playerPoints == 3){
                        playWinningSound();
                        if(!newDisplayUI(pRenderer, currentPointsDisplayUI, player->playerPoints)){
                            printf("Error with newDisplayUI 2: %s\n", SDL_GetError());
                        }
                    }
                }
                break;
            case 2:
                if(handleFlagDelivery(player, flag, map, 5, 82)){ // its possible that we need to add the pPlayer data member positions instead of 5,5
                    printf("Players start position X and Y 1: %d, %d", player->xStart, player->yStart);
                    printf("Player got point: %d\n", player->playerPoints);

                    if(!newDisplayUI(pRenderer, currentPointsDisplayUI, player->playerPoints)){
                        printf("Error with newDisplayUI 1: %s\n", SDL_GetError());
                    }
                    playPlayerGotPointSound();
                    if(player->playerPoints == 3){
                        playWinningSound();
                        // player->playerPoints = 0;
                        if(!newDisplayUI(pRenderer, currentPointsDisplayUI, player->playerPoints)){
                            printf("Error with newDisplayUI 2: %s\n", SDL_GetError());
                        }
                    }
                }
                break;
            case 3:
                if(handleFlagDelivery(player, flag, map, 60, 82)){ // its possible that we need to add the pPlayer data member positions instead of 5,5
                    printf("Players start position X and Y 1: %d, %d", player->xStart, player->yStart);
                    printf("Player got point: %d\n", player->playerPoints);

                    if(!newDisplayUI(pRenderer, currentPointsDisplayUI, player->playerPoints)){
                        printf("Error with newDisplayUI 1: %s\n", SDL_GetError());
                    }
                    playPlayerGotPointSound();
                    if(player->playerPoints == 3){
                        playWinningSound();
                        // player->playerPoints = 0;
                        if(!newDisplayUI(pRenderer, currentPointsDisplayUI, player->playerPoints)){
                            printf("Error with newDisplayUI 2: %s\n", SDL_GetError());
                        }
                    }
                }
                break;
            default:
                break;
    }
    return 1;
}