#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../objects/gridmap.h"

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

//Renders grid map either with pure colors or sprites, check comments in the definiton of the function
void renderGridMap(SDL_Renderer *renderer, GridMap *map, SDL_Texture* texture) 
{
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            SDL_Rect cellRect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            switch (map->cells[y][x].type) {
                case EMPTY:
                    SDL_SetRenderDrawColor(renderer, 0, 153, 0, 255); //leaving this if color is needed instead
                    //SDL_RenderCopy(renderer, texture, NULL, &cellRect); //for rendering sprite
                    break;
                case OBSTACLE:
                    SDL_SetRenderDrawColor(renderer, 0, 204, 204, 255); //leaving this if color is needed instead
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
            SDL_RenderFillRect(renderer, &cellRect); //comment or delete if not using SDL_SetRenderDrawColor
        }
    }
}

//Checks the position under player
void getPlayerGridPosition(int playerX, int playerY, int* gridX, int* gridY) 
{
    *gridX = (playerX + CELL_SIZE/2) / CELL_SIZE;
    *gridY = (playerY + CELL_SIZE/2) / CELL_SIZE;
    //printf("Player position X: %d, Player position Y: %d\n", *gridX, *gridY);
}