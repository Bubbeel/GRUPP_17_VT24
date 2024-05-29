#ifndef gridmap_h
#define gridmap_h

#define CELL_SIZE 32
#define GRID_WIDTH 88
#define GRID_HEIGHT 66

typedef enum 
{
    EMPTY,
    WALL,
    OBSTACLE,
    WEAPON,
    FLAG,
    PLAYER,
    ENEMY,
    SHADE

} CellType;

// Defines the structure to represent a single cell in the grid
typedef struct 
{
    SDL_Rect cellRect;
    CellType type;
    char* tag;
} GridCell;

// Defines the structure to represent the GridMap
typedef struct 
{
    GridCell cells[GRID_HEIGHT][GRID_WIDTH]; // 2D array of grid cells
} GridMap;

GridMap* createGridMap();
void loadMapFromFile(const char* filename, GridMap* map);
SDL_Texture* loadGridMap(SDL_Renderer *renderer);
void renderGridMap(SDL_Renderer *renderer, GridMap* map, SDL_Texture* GridSpriteTexture, int playerX, int playerY, int windowWidth, int windowHeight);
void getPlayerGridPosition(int playerX, int playerY, int* gridX, int* gridY, GridMap* map);
int searchWeaponTile(SDL_Rect* coordinates, GridMap* map);
void destroyGridMap(GridMap* gridMap);

#endif // gridmap_h