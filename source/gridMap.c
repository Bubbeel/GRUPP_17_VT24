// gridMap.c

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "gridMap.h"

// Function to initialize the GridMap
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

// void initializeGridMap(GridMap* map) 
// {
//     for (int x = 0; x < GRID_WIDTH; x++) {
//         for (int y = 0; y < GRID_HEIGHT; y++) {
//             if (x%2 == 0)
//             {
//                 map->cells[x][y].type = EMPTY;
//                 {
//                     if(y%2 == 0)
//                     {
//                         map->cells[x][y].type = OBSTACLE;
//                     }
//                 }
//             }
//             else
//             {
//                 map->cells[x][y].type = OBSTACLE;
//                 if(y%2 == 0)
//                 {
//                     map->cells[x][y].type = EMPTY;
//                 }
//             }
//         }
//     }
// }

SDL_Texture* loadGridMap(SDL_Renderer *renderer)
{
    SDL_Surface* pSurfaceTest = IMG_Load("resources/player1.png");
    if (!pSurfaceTest)
    {
        printf("Error: %s\n", IMG_GetError());
    }
    printf("Surface loaded \n");

    SDL_Texture* pTextureTest = SDL_CreateTextureFromSurface(renderer, pSurfaceTest);
    SDL_FreeSurface(pSurfaceTest);
    if(!pTextureTest)
    {   
        printf("Error: %s\n", SDL_GetError());
    }
    printf("Texture loaded \n");

    return pTextureTest;
}

void renderGridMap(SDL_Renderer *renderer, GridMap *map, SDL_Texture* texture) 
{
    // int windowWidth, windowHeight;
    // SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            SDL_Rect cellRect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            switch (map->cells[y][x].type) {
                case EMPTY:
                    SDL_SetRenderDrawColor(renderer, 0, 153, 0, 255); //leaving this if color is needed instead
                    //SDL_RenderCopy(renderer, texture, NULL, &cellRect);
                    break;
                case OBSTACLE:
                    SDL_SetRenderDrawColor(renderer, 0, 204, 204, 255); //leaving this if color is needed instead
                    //SDL_RenderCopy(renderer, texture, NULL, &cellRect);
                    break;
                case FLAG:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0 ,255);
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    break;
                // Add more cases for other cell types
            }
            SDL_RenderFillRect(renderer, &cellRect);
        }
    }
}

// WIP!!! Need to figure out how to check position under player
// void positionOnGrid()
// {
//     printf("TileX: %d, TileY: %d", map->cells[]);
// }