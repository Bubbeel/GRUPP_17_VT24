#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "gridMap.h"
#include "player.h"
#include "collisionDetection.h"

GridMap* createGridMap()
{
    GridMap* gridMap = malloc(sizeof(GridMap));
    if(!gridMap)
    {
        return NULL;
    }
    printf("Succeded in gridMap memory\n");
    return gridMap;

}

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

    SDL_Surface* gridSpriteSurface = IMG_Load("../lib/resources/MapFullSpriteSheet_v1.png");
    if (!gridSpriteSurface)
    {
        printf("Error: %s\n", IMG_GetError());
    }

    SDL_Texture* gridSpriteTexture = SDL_CreateTextureFromSurface(renderer, gridSpriteSurface);
    SDL_FreeSurface(gridSpriteSurface);
    if(!gridSpriteTexture)
    {   
        printf("Error: %s\n", SDL_GetError());
    }

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
                        srcRect.x = 0;
                        srcRect.y = 0;
                        break;
                    case WALL:
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
                        srcRect.x = CELL_SIZE;
                        break;
                    case WEAPON:
                        break;
                    default:
                        break;
                }
                SDL_Rect destRect = {screenX, screenY, CELL_SIZE, CELL_SIZE};
                SDL_RenderCopy(renderer, GridSpriteTexture, &srcRect, &destRect);

                switch(map->cells[y][x].type) {
                    case EMPTY:
                        if (strcmp("shade", map->cells[y][x].tag) == 0){ // 16
                            srcRect.x = 0;
                            srcRect.y = CELL_SIZE * 5;
                        }
                        else if(strcmp("shade1", map->cells[y][x].tag) == 0){ // 10
                            srcRect.x = 0;
                            srcRect.y = CELL_SIZE*3;
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

//Checks the position under player
void getPlayerGridPosition(int playerX, int playerY, int* gridX, int* gridY, GridMap* map) 
{
    *gridX = (playerX + CELL_SIZE/2) / CELL_SIZE;
    *gridY = (playerY + CELL_SIZE/2) / CELL_SIZE;
    //printf("Player grid position X: %d, Player grid position Y: %d\n", *gridX, *gridY/*map->cells[*gridX][*gridY].cellRect.x, map->cells[*gridX][*gridY].cellRect.y*/);
    //uncomment printf if needed
}

// The code is used to find the tile that a pixel is in. It takes the x and y position of the pixel and divides each by 3, and then adds 32 to each result. It then returns the result of each division. The result is the tile that the pixel is in.
int searchWeaponTile(SDL_Rect* coordinates, GridMap* map) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (map->cells[y][x].type == WEAPON) {
                coordinates->x = map->cells[y][x].cellRect.x;
                coordinates->y = map->cells[y][x].cellRect.y;
                printf("Found WEAPON at (%d, %d)\n", coordinates->x, coordinates->y);
                map->cells[y][x].type = EMPTY;
                map->cells[y][x].tag = "Floor";
                return 1; 
            }
        }
    }
   return 0;
}

void destroyGridMap(GridMap* gridMap) 
{
    free(gridMap);
}