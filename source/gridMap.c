// gridMap.c

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "gridMap.h"

// Function to initialize the GridMap
void initializeGridMap(GridMap* map) {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            // Set all cells to empty initially
            if (x%2 == 0)
            {
                map->cells[x][y].type = EMPTY;
                {
                    if(y%2 == 0)
                    {
                        map->cells[x][y].type = OBSTACLE;
                    }
                }
            }
            else
            {
                map->cells[x][y].type = OBSTACLE;
                if(y%2 == 0)
                {
                    map->cells[x][y].type = EMPTY;
                }
            }
        }
    }
}

void renderGridMap(SDL_Renderer *renderer, GridMap *map) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            SDL_Rect cellRect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            switch (map->cells[x][y].type) {
                case EMPTY:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set color to white
                    break;
                case OBSTACLE:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set color to black
                    break;
                // Add more cases for other cell types as needed
            }
            SDL_RenderFillRect(renderer, &cellRect);
        }
    }
}