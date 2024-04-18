// gridMap.h

#ifndef gridmap_h
#define gridmap_h

// Define the size of the grid
#define GRID_WIDTH 70
#define GRID_HEIGHT 40
#define CELL_SIZE 16

// Types of data that can be stored in each grid cell
typedef enum 
{
    EMPTY,
    OBSTACLE,
    WEAPON,
    FLAG,
    PLAYER,
    ENEMY
} CellType;

// Defines the structure to represent a single cell in the grid
typedef struct 
{
    CellType type;
    // Add additional fields as needed
} GridCell;

// Defines the structure to represent the GridMap
typedef struct 
{
    GridCell cells[GRID_WIDTH][GRID_HEIGHT]; // 2D array of grid cells
} GridMap;

// Declare function prototypes
void loadMapFromFile(const char* filename, GridMap* map);
//void initializeGridMap(GridMap* map);
SDL_Texture* loadGridMap(SDL_Renderer *renderer);
void renderGridMap(SDL_Renderer *renderer, GridMap* map, SDL_Texture* texture);

#endif // gridmap_h