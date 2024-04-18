// gridMap.h

#ifndef GRIDMAP_H
#define GRIDMAP_H

// Define the size of the grid
#define GRID_WIDTH 50
#define GRID_HEIGHT 40
#define CELL_SIZE 16

// Define the types of data that can be stored in each grid cell
typedef enum {
    EMPTY,
    OBSTACLE,
    WEAPON,
    FLAG,
    PLAYER,
    ENEMY
} CellType;

// Define the structure to represent a single cell in the grid
typedef struct {
    CellType type;
    // Add additional fields as needed
} GridCell;

// Define the structure to represent the GridMap
typedef struct {
    GridCell cells[GRID_WIDTH][GRID_HEIGHT]; // 2D array of grid cells
} GridMap;

// Declare function prototypes
void initializeGridMap(GridMap* map);
void renderGridMap(SDL_Renderer *renderer, GridMap* map);

#endif // GRIDMAP_H