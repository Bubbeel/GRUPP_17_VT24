#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "gridmap.h"
#include "player.h"
#include "collisionDetection.h"
//#include "gameObject.h"

GridMap* createGridMap()
{
    GridMap* gridMap = malloc(sizeof(GridMap));
    if(!gridMap)
    {
        return NULL;
    }
    return gridMap;

}

//
// Function to read map design from a textfile and initialize the GridMap
void loadMapFromFile(const char* filename, GridMap* map)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening file\n");
        return;
    }
    for(int y = 0; y < GRID_HEIGHT; y++)
    {
        for(int x = 0; x < GRID_WIDTH; x++)
        {
            char cellType;
            if(fscanf(file, " %c", &cellType) != 1)
            {
                printf("Error reading file \n");
                fclose(file);
                return;
            }
            switch(cellType)
            {
                case 'E':
                    map->cells[y][x].type = EMPTY;
                    map->cells[y][x].tag = "Floor";
                    break;
                case 'O':
                    map->cells[y][x].type = WALL;
                    map->cells[y][x].tag = "Wall";
                    break;
                case 'N':
                    map->cells[y][x].type = WALL;
                    map->cells[y][x].tag = "Wall1";
                    break;
                case 'P':
                    map->cells[y][x].type = WALL;
                    map->cells[y][x].tag = "Wall2";
                    break;
                case 'Q':
                    map->cells[y][x].type = WALL;
                    map->cells[y][x].tag = "Wall3";
                    break;
                case 'W':
                    map->cells[y][x].type = WEAPON;
                    map->cells[y][x].tag = "Item";
                    //map->cells[y][x].spriteType = 1;
                    break;
                case 'S':
                    map->cells[y][x].type = SHADE;
                    map->cells[y][x].tag = "shade";
                    break;
                case 'T':
                    map->cells[y][x].type = EMPTY;
                    map->cells[y][x].tag = "shade1";
                    break;
                case 'U':
                    map->cells[y][x].type = EMPTY;
                    map->cells[y][x].tag = "shade2";
                    break;
                case 'V':
                    map->cells[y][x].type = EMPTY;
                    map->cells[y][x].tag = "shade3";
                    break;
                case 'X':
                    map->cells[y][x].type = EMPTY;
                    map->cells[y][x].tag = "shade4";
                    break;
                case 'Y':
                    map->cells[y][x].type = EMPTY;
                    map->cells[y][x].tag = "shade5";
                    break;
                case 'Z':
                    map->cells[y][x].type = EMPTY;
                    map->cells[y][x].tag = "shade6";
                    break;
                //add more later if needed
                default:
                    printf("Unknown cell type in file \n");
                    fclose(file);
                    return; 
            }
        }
    } 
    fclose(file);
    return;
}

//Loads grid map
//Keep in mind that this function is of type SDL_Texture* so it must return a SDL_Texture* which we then use in main 
//Currently loads only one sprite, thinking about a way to render multiple sprites instead?
//If anybody wants to you can play around with this function, just make sure to make a copy of this version thx
SDL_Texture* loadGridMap(SDL_Renderer *renderer)
{

    SDL_Surface* gridSpriteSurface = IMG_Load("../lib/resources/MapFullSpriteSheet_v1.png");
    if (!gridSpriteSurface)
    {
        printf("Error: %s\n", IMG_GetError());
    }
    /*
    SDL_Surface* pSurfaceTest = IMG_Load("resources/player1.png");
    if (!pSurfaceTest)
    {
        printf("Error: %s\n", IMG_GetError());
    }
    //printf("Surface loaded \n");
    */
    SDL_Texture* gridSpriteTexture = SDL_CreateTextureFromSurface(renderer, gridSpriteSurface);
    SDL_FreeSurface(gridSpriteSurface);
    if(!gridSpriteTexture)
    {   
        printf("Error: %s\n", SDL_GetError());
    }
    /*
    SDL_Texture* pTextureTest = SDL_CreateTextureFromSurface(renderer, pSurfaceTest);
    SDL_FreeSurface(pSurfaceTest);
    if(!pTextureTest)
    {   
        printf("Error: %s\n", SDL_GetError());
    }
    //printf("Texture loaded \n");
    */
    return gridSpriteTexture;
}

void renderGridMap(SDL_Renderer *renderer, GridMap* map, SDL_Texture* GridSpriteTexture, int playerX, int playerY, int windowWidth, int windowHeight) 
{
    SDL_Rect srcRect = {0, 0, CELL_SIZE, CELL_SIZE};

    for (int y = 0; y < GRID_HEIGHT; y++) 
    {
        for (int x = 0; x < GRID_WIDTH; x++) 
        {
            // Calculate screen position based on player's position
            int screenX = map->cells[y][x].cellRect.x - playerX;
            int screenY = map->cells[y][x].cellRect.y - playerY;
            SDL_Rect cellRect = { screenX, screenY, CELL_SIZE, CELL_SIZE };
            // Only render cells within the screen bounds
            if (screenX + CELL_SIZE >= 0 && screenX <= windowWidth && screenY + CELL_SIZE >= 0 && screenY <= windowHeight) 
            {
                switch (map->cells[y][x].type) {
                    case EMPTY:
                        //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        srcRect.x = 0;
                        srcRect.y = 0;
                        break;
                    case WALL:
                        //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        if (strcmp("Wall", map->cells[y][x].tag) == 0){
                            srcRect.y = CELL_SIZE;
                            srcRect.x = 0;
                        }
                        else if(strcmp("Wall1", map->cells[y][x].tag) == 0){
                            srcRect.y = CELL_SIZE;
                            srcRect.x = CELL_SIZE * 2;
                        }
                        else if(strcmp("Wall2", map->cells[y][x].tag) == 0){
                            srcRect.y = CELL_SIZE*2;
                            srcRect.x = 0;
                        }
                        else if(strcmp("Wall3", map->cells[y][x].tag) == 0){
                            srcRect.y = CELL_SIZE*2;
                            srcRect.x = CELL_SIZE;
                        }
                        break;
                    case FLAG:
                        //SDL_SetRenderDrawColor(renderer, 255, 255, 0 ,255);
                        srcRect.x = CELL_SIZE;
                        break;
                    case WEAPON:
                        //SDL_SetRenderDrawColor(renderer, 255, 255, 0 ,255);
                        break;
                    default:
                        //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        srcRect.x = 0;
                        break;
                }
                //SDL_Rect cellRect = { screenX, screenY, CELL_SIZE, CELL_SIZE };
                //SDL_RenderFillRect(renderer, &cellRect);
                SDL_Rect destRect = {screenX, screenY, CELL_SIZE, CELL_SIZE};
                SDL_RenderCopy(renderer, GridSpriteTexture, &srcRect, &destRect);
                /*if (strcmp(map->cells[y][x].tag, "shade1") == 0) {
                    srcRect.x = 0;
                    srcRect.y = CELL_SIZE * 3;
                }
                else if(strcmp(map->cells[y][x].tag, "shade2") == 0) {
                    srcRect.x = CELL_SIZE;
                    srcRect.y = CELL_SIZE * 3;
                }
                else if(strcmp(map->cells[y][x].tag, "shade3") == 0) {
                    srcRect.x = CELL_SIZE * 2;
                    srcRect.y = CELL_SIZE * 3;
                }
                else if(strcmp(map->cells[y][x].tag, "shade4") == 0) {
                    srcRect.x = 0;
                    srcRect.y = CELL_SIZE * 4;
                }
                else if(strcmp(map->cells[y][x].tag, "shade5") == 0) {
                    srcRect.x = CELL_SIZE;
                    srcRect.y = CELL_SIZE * 4;
                }
                else if(strcmp(map->cells[y][x].tag, "shade6") == 0) {
                    srcRect.x = CELL_SIZE * 2;
                    srcRect.y = CELL_SIZE * 4;
                }
                else {
                    srcRect.x = CELL_SIZE;
                    srcRect.y = CELL_SIZE * 5;
                }
                SDL_RenderCopy(renderer, GridSpriteTexture, &srcRect, &destRect);*/
                // map->cells[y][x].cellRect.x = screenX;
                // map->cells[y][x].cellRect.y = screenY;
                // map->cells[y][x].cellRect.w = CELL_SIZE;
                // map->cells[y][x].cellRect.h = CELL_SIZE;
                // SDL_RenderFillRect(renderer, &map->cells[y][x].cellRect);


                switch(map->cells[y][x].type) {
                    case EMPTY:
                        if (strcmp("shade", map->cells[y][x].tag) == 0){ // 16
                            srcRect.x = 0;
                            srcRect.y = CELL_SIZE * 5;
                        }
                        else if(strcmp("shade1", map->cells[y][x].tag) == 0){ // 10
                            srcRect.x = 0;
                            srcRect.y = CELL_SIZE*3;
                            //srcRect.x = CELL_SIZE;
                            //srcRect.y = CELL_SIZE * 3;
                        }
                        else if(strcmp("shade2", map->cells[y][x].tag) == 0){ // 11
                            srcRect.x = CELL_SIZE;
                            srcRect.y = CELL_SIZE * 3;
                        }
                        else if(strcmp("shade3", map->cells[y][x].tag) == 0){ // 12
                            srcRect.x = CELL_SIZE * 2;
                            srcRect.y = CELL_SIZE * 3;
                        }
                        else if(strcmp("shade4", map->cells[y][x].tag) == 0){ // 13
                            srcRect.x = 0;
                            srcRect.y = CELL_SIZE * 4;
                        }
                        else if(strcmp("shade5", map->cells[y][x].tag) == 0){ // 14
                            srcRect.x = CELL_SIZE;
                            srcRect.y = CELL_SIZE * 4;
                        }
                        else if(strcmp("shade6", map->cells[y][x].tag) == 0){ // 15
                            srcRect.x = CELL_SIZE * 2;
                            srcRect.y = CELL_SIZE * 4;
                        }
                        break;
                    default:
                    srcRect.x = CELL_SIZE;
                    srcRect.y = CELL_SIZE * 5;
                    break;
                }
                SDL_RenderCopy(renderer, GridSpriteTexture, &srcRect, &destRect);

            }
        }
    }
}

// New function to render grid map centered around player
void renderGridMapCentered(SDL_Renderer *renderer, GridMap* map, SDL_Texture* gridTexture, Player* player, int windowWidth, int windowHeight, int levelWidth, int levelHeight) {
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
    renderGridMap(renderer, map, gridTexture,player->camera.x, player->camera.y, windowWidth, windowHeight);
}


// void renderVisibleMap(SDL_Renderer *renderer, GridMap *map, Player* player, int screenWidth, int screenHeight) 
// {
//     // Calculate the top-left corner of the visible area based on the camera's position
//     int cameraX = player->playerX - (screenWidth /2); //remove /2 if error fixing, not sure what it does
//     int cameraY = player->playerY - (screenHeight /2);

//     // Ensure the camera stays within the bounds of the map
//     if (cameraX < 0) 
//     {
//         cameraX = 0;
//     }
//     if (cameraY < 0) 
//     {
//         cameraY = 0;
//     }
//     if (cameraX > GRID_WIDTH*CELL_SIZE - screenWidth) 
//     {
//         cameraX = GRID_WIDTH*CELL_SIZE - screenWidth;
//     }
//     if (cameraY > GRID_HEIGHT*CELL_SIZE - screenHeight) 
//     {
//         cameraY = GRID_HEIGHT*CELL_SIZE - screenHeight;
//     }

//     // Calculate the position of the camera relative to the player
//     int offsetX = player->playerX - cameraX;
//     int offsetY = player->playerY - cameraY;
//     printf("OffsetX: %d, OffsetY: %d \n", offsetX, offsetY);


//     // Render only the portion of the map that is visible on the screen
//     for (int y = cameraY; y < screenHeight/CELL_SIZE; y++) 
//     {
//         for (int x = cameraX; x < screenWidth/CELL_SIZE; x++) 
//         {
//             // Calculate the position of the cell on the screen
//             int mapX = cameraX + x;
//             int mapY = cameraY + y;
//             if (mapX >= 0 && mapX < GRID_WIDTH && mapY >= 0 && mapY < GRID_HEIGHT) 
//             {
//                 // Calculate the screen position of the cell
//                 int screenX = x * CELL_SIZE - offsetX;
//                 int screenY = y * CELL_SIZE - offsetY;

//                 // Render the cell at the calculated screen position
//                 SDL_Rect cellRect = {screenX, screenY, CELL_SIZE, CELL_SIZE};
//                 switch (map->cells[mapY][mapX].type) 
//                 {
//                     case EMPTY:
//                         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Render empty cell as white
//                         break;
//                     case OBSTACLE:
//                         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Render obstacle cell as black
//                         break;
//                     case FLAG:
//                         SDL_SetRenderDrawColor(renderer, 255, 255, 0 ,255); // Render flag cell as yellow
//                         break;
//                     default:
//                         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Default to white for unknown cell types
//                         break;
//                 }
//                 SDL_RenderFillRect(renderer, &cellRect); // Render the cell
//             }  
//         }
//     }
// }

int searchWeaponTile(SDL_Rect* coordinates, GridMap* map) {
    // The code is used to find the tile that a pixel is in. It takes the x and y position of the pixel and divides each by 3, and then adds 32 to each result. It then returns the result of each division. The result is the tile that the pixel is in.
    //int tileX = y / CELL_SIZE;
    //int tileY = x / CELL_SIZE;
    printf("Inside\n");
    // Check if the calculated tile coordinates are within the grid boundaries

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            // Check the type of the tile at the calculated coordinates
            if (map->cells[y][x].type == WEAPON) {
                // Found a weapon tile
                coordinates->x = map->cells[y][x].cellRect.x;
                coordinates->y = map->cells[y][x].cellRect.y;
                printf("Found WEAPON at (%d, %d)\n", coordinates->x, coordinates->y);
                map->cells[y][x].type = EMPTY;
                map->cells[y][x].tag = "Floor";
                return 1;  // Exit the function as soon as we find the tile
            }
        }
    }
    printf("it fucked it\n");
    /*
    for (int y = 0; y < GRID_HEIGHT; y++) 
    {
        for (int x = 0; x < GRID_WIDTH; x++) 
        {
            // Check the type of the tile at the calculated coordinates
            if (map->cells[x][y].type == EMPTY) {
                // Collision detected with a weapon tile
                printf("Found WEAPON");
                coordinates.x = x / CELL_SIZE;
                coordinates.y = y / CELL_SIZE;
                return 1;
            }
        }

    }
    */
    return 0;
}

//Checks the position under player
void getPlayerGridPosition(int playerX, int playerY, int* gridX, int* gridY, GridMap* map) 
{
    *gridX = (playerX + CELL_SIZE/2) / CELL_SIZE;
    *gridY = (playerY + CELL_SIZE/2) / CELL_SIZE;
    //printf("Player grid position X: %d, Player grid position Y: %d\n", *gridX, *gridY/*map->cells[*gridX][*gridY].cellRect.x, map->cells[*gridX][*gridY].cellRect.y*/);
}

void destroyGridMap(GridMap* gridMap) 
{
    free(gridMap);
}