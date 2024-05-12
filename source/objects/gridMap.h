// gridMap.h

#ifndef gridmap_h
#define gridmap_h

// !!!IMPORTANT Define the size of the grid IMPORTANT!!!
//Currently the sizes do not scale automatically to the window size, they are hardcoded
//It is planned to change that in the future if possible
//If there are any changes to the resolution make sure to divide width/heigth by CELL_SIZE 
//to get your GRID_WIDTH/GRID_HEIGTH
//This is !important! because those parameters decide how much of the textfile for map is going to be read
//If those values become bigger than what is written in the textfile then there will be a mess with the rendering
//and a lot of black boxes will appear instead
//Tested and player.png should be 640x640 pixels to be fit into a squre of cell_size 32, but gameplay-wise 
//I think its better to have 600x600 or less - Konrad 

#define CELL_SIZE 32
#define GRID_WIDTH 88
#define GRID_HEIGHT 66

#include "player.h"

// Types of data that can be stored in each grid cell
//Feel free to add and remove types if you need, but dont forget to assign them functionality in gridMap.c
typedef enum 
{
    EMPTY,
    WALL,
    OBSTACLE,
    WEAPON,
    FLAG,
    PLAYER,
    ENEMY
    //add more if needed, remember that enum is numering objects from 0 to X, 
    //so the highest object on the list is always 0
} CellType;

// Defines the structure to represent a single cell in the grid
typedef struct 
{
    SDL_Rect cellRect;
    CellType type;
    char* tag;
    // Add additional fields if needed
} GridCell;

// Defines the structure to represent the GridMap
typedef struct 
{
    GridCell cells[GRID_HEIGHT][GRID_WIDTH]; // 2D array of grid cells
} GridMap;

GridMap* createGridMap();
void loadMapFromFile(const char* filename, GridMap* map);
SDL_Texture* loadGridMap(SDL_Renderer *renderer);
void renderVisibleMap(SDL_Renderer *renderer, GridMap *map, Player* player, int screenWidth, int screenHeight);
void renderGridMap(SDL_Renderer *renderer, GridMap* map, int playerX, int playerY, int windowWidth, int windowHeight);
void renderGridMapCentered(SDL_Renderer *renderer, GridMap* map, Player* player, int windowWidth, int windowHeight, int levelWidth, int levelHeight);
void getPlayerGridPosition(int playerX, int playerY, int* gridX, int* gridY, GridMap* map);
void destroyGridMap(GridMap* gridMap);

#endif // gridmap_h