#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "gridMap.h"
#include "objects/player.h"

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
                    break;
                case 'O':
                    map->cells[y][x].type = OBSTACLE;
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


// void renderVisibleMap(SDL_Renderer *renderer, GridMap *map, int playerX, int playerY, int screenWidth, int screenHeight) {
//     // Calculate the top-left corner of the visible area based on the camera's position
//     int cameraX = playerX - (screenWidth / 2);
//     int cameraY = playerY - (screenHeight / 2);

//     // Ensure the camera stays within the bounds of the map
//     if (cameraX < 0) {
//         cameraX = 0;
//         //printf("CameraX: %d, PlayerX: %d\n", cameraX, playerX);
//     }
//     if (cameraY < 0) {
//         cameraY = 0;
//     }
//     if (cameraX > GRID_WIDTH /*- screenWidth*/) {
//         cameraX = GRID_WIDTH /*- screenWidth*/;
//     }
//     if (cameraY > GRID_HEIGHT /*- screenHeight*/) {
//         cameraY = GRID_HEIGHT /*- screenHeight*/;
//     }
//     // Render only the portion of the map that is visible on the screen
//     for (int y = cameraY; y < GRID_HEIGHT; y++) {
//         for (int x = cameraX; x < GRID_WIDTH; x++) {
//             // Calculate the position of the cell on the screen
//             int screenX = (x - cameraX) * CELL_SIZE;
//             int screenY = (y - cameraY) * CELL_SIZE;
//             // Render the cell at the calculated screen position
//             //SDL_Rect cellRect = {screenX, screenY, CELL_SIZE, CELL_SIZE};
//             switch (map->cells[y][x].type) {
//                 case EMPTY:
//                     SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Render empty cell as white
//                     break;
//                 case OBSTACLE:
//                     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Render obstacle cell as black
//                     break;
//                 case FLAG:
//                     SDL_SetRenderDrawColor(renderer, 255, 255, 0 ,255); // Render flag cell as yellow
//                     break;
//                 default:
//                     SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Default to white for unknown cell types
//                     break;
//             }
//             // map->cells[y][x].cellRect.x = screenX;
//             // map->cells[y][x].cellRect.y = screenY;
//             // map->cells[y][x].cellRect.w = CELL_SIZE;
//             // map->cells[y][x].cellRect.h = CELL_SIZE;
//             //printf("ScreenX: %d, ScreenY: %d\n", screenX, screenY);
//             SDL_RenderFillRect(renderer, &map->cells[y][x].cellRect); //comment or delete if not using SDL_SetRenderDrawColor
//         }
//     }
// }

// Function to render the portion of the map visible on the screen based on the player's position
void renderVisibleMap(SDL_Renderer *renderer, GridMap *map, int playerX, int playerY, int screenWidth, int screenHeight) {
    // Calculate the top-left corner of the visible area based on the player's position
    int topLeftX = playerX - (screenWidth / 2);
    int topLeftY = playerY - (screenHeight / 2);

    // Ensure the top-left corner is within the bounds of the map
    if (topLeftX < 0) {
        topLeftX = 0;
    }
    if (topLeftY < 0) {
        topLeftY = 0;
    }


    // Calculate the bottom-right corner of the visible area
    int bottomRightX = topLeftX + screenWidth;
    int bottomRightY = topLeftY + screenHeight;
    printf("botrighttX: %d, botrightY; %d\n", bottomRightX, bottomRightY);

    // Ensure the bottom-right corner is within the bounds of the map
    if (bottomRightX > GRID_WIDTH) {
        bottomRightX = GRID_WIDTH;
    }
    if (bottomRightY > GRID_HEIGHT) {
        bottomRightY = GRID_HEIGHT;
    }

    //printf("bottomRightX: %d, bottomRightY: %d \n", bottomRightX, bottomRightY);

    // Render only the portion of the map that is visible on the screen
    for (int y = topLeftY; y < bottomRightY; y++) {
        for (int x = topLeftX; x < bottomRightX; x++) {
            // Calculate the position of the cell on the screen
            int screenX = (x - topLeftX) * CELL_SIZE;
            int screenY = (y - topLeftY) * CELL_SIZE;

            // Render the cell at the calculated screen position
            //SDL_Rect cellRect = {screenX, screenY, CELL_SIZE, CELL_SIZE};
            switch (map->cells[y][x].type) {
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
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); //if there are errors, there will be rg squares
                    break;
                // Add more cases for other cell types
            }
            // map->cells[y][x].cellRect.x = screenX;
            // map->cells[y][x].cellRect.y = screenY;
            // map->cells[y][x].cellRect.w = CELL_SIZE;
            // map->cells[y][x].cellRect.h = CELL_SIZE;
            //printf("ScreenX: %d, ScreenY: %d\n", screenX, screenY);
            SDL_RenderFillRect(renderer, &map->cells[y][x].cellRect);
        }
    }
}

//Renders grid map either with pure colors or sprites, check comments in the definiton of the function
void renderGridMap(SDL_Renderer *renderer, GridMap *map, SDL_Texture* texture) 
{
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            switch (map->cells[y][x].type) {
                case EMPTY:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //leaving this if color is needed instead
                    //SDL_RenderCopy(renderer, texture, NULL, &cellRect); //for rendering sprite
                    break;
                case OBSTACLE:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //leaving this if color is needed instead
                    //SDL_RenderCopy(renderer, texture, NULL, &cellRect); //for rendering sprite
                    break;
                case FLAG:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0 ,255); //leaving this if color is needed instead
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //if there are errors, there will be black squares
                    break;
                // Add more cases for other cell types
            }
            map->cells[y][x].cellRect.x = x * CELL_SIZE;
            map->cells[y][x].cellRect.y = y * CELL_SIZE;
            map->cells[y][x].cellRect.w = CELL_SIZE;
            map->cells[y][x].cellRect.h = CELL_SIZE;
            //printf("ScreenX: %d, ScreenY: %d\n", screenX, screenY);
            SDL_RenderFillRect(renderer, &map->cells[y][x].cellRect); //comment or delete if not using SDL_SetRenderDrawColor
        }
    }
}

//Checks the position under player
void getPlayerGridPosition(int playerX, int playerY, int* gridX, int* gridY, GridMap* map) 
{
    *gridX = (playerX + CELL_SIZE/2) / CELL_SIZE;
    *gridY = (playerY + CELL_SIZE/2) / CELL_SIZE;
    //printf("Player grid position X: %d, Player grid position Y: %d\n", map->cells[*gridX][*gridY].cellRect.x, map->cells[*gridX][*gridY].cellRect.y);
}

void destroyGridMap(GridMap* gridMap) 
{
    free(gridMap);
}