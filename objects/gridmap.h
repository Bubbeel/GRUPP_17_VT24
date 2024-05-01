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

#define CELL_SIZE 32
#define GRID_WIDTH 44
#define GRID_HEIGHT 25

// Types of data that can be stored in each grid cell
//Feel free to add and remove types if you need, but dont forget to assign them functionality in gridMap.c
typedef enum 
{
    EMPTY,
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
    CellType type;
    // Add additional fields if needed
} GridCell;

// Defines the structure to represent the GridMap
typedef struct 
{
    GridCell cells[GRID_HEIGHT][GRID_WIDTH]; // 2D array of grid cells
} GridMap;


void loadMapFromFile(const char* filename, GridMap* map);

SDL_Texture* loadGridMap(SDL_Renderer *renderer);

void renderGridMap(SDL_Renderer *renderer, GridMap* map, SDL_Texture* texture);

void getPlayerGridPosition(int playerX, int playerY, int* gridX, int* gridY);

#endif // gridmap_h