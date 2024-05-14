#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../objects/gridmap.h"
#include "../objects/player.h"
#include "../objects/collisionDetection.h"
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
            map->cells[y][x].cellRect.x = x * CELL_SIZE;
            map->cells[y][x].cellRect.y = y * CELL_SIZE;
            map->cells[y][x].cellRect.w = CELL_SIZE;
            map->cells[y][x].cellRect.h = CELL_SIZE;
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
                case 'F':
                    map->cells[y][x].type = FLAG;
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
}

//Loads grid map
//Keep in mind that this function is of type SDL_Texture* so it must return a SDL_Texture* which we then use in main 
//Currently loads only one sprite, thinking about a way to render multiple sprites instead?
//If anybody wants to you can play around with this function, just make sure to make a copy of this version thx
SDL_Texture* loadGridMap(SDL_Renderer *renderer)
{
    SDL_Surface* pSurfaceTest = IMG_Load("resources/player1.png");
    if (!pSurfaceTest)
    {
        printf("Error: %s\n", IMG_GetError());
    }
    //printf("Surface loaded \n");

    SDL_Texture* pTextureTest = SDL_CreateTextureFromSurface(renderer, pSurfaceTest);
    SDL_FreeSurface(pSurfaceTest);
    if(!pTextureTest)
    {   
        printf("Error: %s\n", SDL_GetError());
    }
    //printf("Texture loaded \n");

    return pTextureTest;
}

void renderGridMap(SDL_Renderer *renderer, GridMap* map, int playerX, int playerY, int windowWidth, int windowHeight) 
{
    for (int y = 0; y < GRID_HEIGHT; y++) 
    {
        for (int x = 0; x < GRID_WIDTH; x++) 
        {
            // Calculate screen position based on player's position
            int screenX = map->cells[y][x].cellRect.x - playerX;
            int screenY = map->cells[y][x].cellRect.y - playerY;

            // Only render cells within the screen bounds
            if (screenX + CELL_SIZE >= 0 && screenX <= windowWidth && screenY + CELL_SIZE >= 0 && screenY <= windowHeight) 
            {
                switch (map->cells[y][x].type) {
                    case EMPTY:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        break;
                    case WALL:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        break;
                    case FLAG:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 0 ,255);
                        break;
                    default:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        break;
                }
                SDL_Rect cellRect = { screenX, screenY, CELL_SIZE, CELL_SIZE };
                SDL_RenderFillRect(renderer, &cellRect);
                // map->cells[y][x].cellRect.x = screenX;
                // map->cells[y][x].cellRect.y = screenY;
                // map->cells[y][x].cellRect.w = CELL_SIZE;
                // map->cells[y][x].cellRect.h = CELL_SIZE;
                // SDL_RenderFillRect(renderer, &map->cells[y][x].cellRect);
            }
        }
    }
}

// New function to render grid map centered around player
void renderGridMapCentered(SDL_Renderer *renderer, GridMap* map, Player* player, int windowWidth, int windowHeight, int levelWidth, int levelHeight) {
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
    renderGridMap(renderer, map, player->camera.x, player->camera.y, windowWidth, windowHeight);
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

//Renders grid map either with pure colors or sprites, check comments in the definiton of the function
// void renderGridMap(SDL_Renderer *renderer, GridMap *map, SDL_Texture* texture) 
// {
//     for (int y = 0; y < GRID_HEIGHT; y++) {
//         for (int x = 0; x < GRID_WIDTH; x++) {
//             switch (map->cells[y][x].type) {
//                 case EMPTY:
//                     SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //leaving this if color is needed instead
//                     //SDL_RenderCopy(renderer, texture, NULL, &cellRect); //for rendering sprite
//                     break;
//                 case WALL:
//                     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //leaving this if color is needed instead
//                     //SDL_RenderCopy(renderer, texture, NULL, &cellRect); //for rendering sprite
//                     break;
//                 case FLAG:
//                     SDL_SetRenderDrawColor(renderer, 255, 255, 0 ,255); //leaving this if color is needed instead
//                     break;
//                 default:
//                     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //if there are errors, there will be black squares
//                     break;
//                 // Add more cases for other cell types
//             }
//             map->cells[y][x].cellRect.x = x * CELL_SIZE;
//             map->cells[y][x].cellRect.y = y * CELL_SIZE;
//             map->cells[y][x].cellRect.w = CELL_SIZE;
//             map->cells[y][x].cellRect.h = CELL_SIZE;
//             //printf("ScreenX: %d, ScreenY: %d\n", screenX, screenY);
//             SDL_RenderFillRect(renderer, &map->cells[y][x].cellRect); //comment or delete if not using SDL_SetRenderDrawColor
//         }
//     }
// }

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