/*#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "objects/player.h"
#include "objects/flag.h"
#include "gridMap.h"
#include "objects/server.h"
#include "objects/client.h"


#define SPEED 100
#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

#define CLOSE_DISTANCE_THRESHOLD 10
#define FLAG_SPEED 2

typedef struct
{
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Surface surface;
    char tag;
    Player
} GameObject;


int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    int flagX, flagY;

    SDL_Window* pWindow = SDL_CreateWindow("CTF Gaming", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!pWindow) {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // GameObject object;
    // object.tag = "enemy";

    // GameObject player;
    // player.tag = "player";

    // GameObject wall;
    // wall.tag = "obstacle";

    // GameObject projectile;
    // projectile.tag = "projectile";

    SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pRenderer) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    Player* pPlayer = createPlayer(pRenderer, SPEED);
    if (!pPlayer) {
        printf("Failed to create player, Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    bool isServer = false;

    // Controll if the first instance of the application is the server
    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        isServer = true;
    }

    if (isServer) {
        // Start the server part
        // Code to start the server
    } else {
        // start client part
        // code to start client
    }

    //background data
    SDL_Surface* pSurface1 = IMG_Load("resources/player1.png");
    SDL_Surface* pSurface2 = IMG_Load("resources/Ship.png");
    SDL_Surface* pSurfaceFlag = IMG_Load("resources/flag.png");

    int gridX, gridY;

    //create GridMap obj and initialize GridMap
    GridMap map;
    //CHECK THE FILE TO MAKE SURE EVERYTHING IS OKAY
    loadMapFromFile("resources/map.txt", &map);
    SDL_Texture* gridTexture;
    gridTexture = loadGridMap(pRenderer);


    if (!pSurface1 || !pSurface2 || !pSurfaceFlag) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* pTexture1 = SDL_CreateTextureFromSurface(pRenderer, pSurface1);
    SDL_Texture* pTexture2 = SDL_CreateTextureFromSurface(pRenderer, pSurface2);
    SDL_Texture* pTextureFlag = SDL_CreateTextureFromSurface(pRenderer, pSurfaceFlag);
    SDL_FreeSurface(pSurface1);
    SDL_FreeSurface(pSurface2);
    SDL_FreeSurface(pSurfaceFlag);

    if (!pTexture1 || !pTexture2 || !pTextureFlag) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Rect playerRect1;
    SDL_Rect playerRect2;
    SDL_Rect flagRect;
    SDL_QueryTexture(pTexture1, NULL, NULL, &playerRect1.w, &playerRect1.h);
    SDL_QueryTexture(pTexture2, NULL, NULL, &playerRect2.w, &playerRect2.h);
    SDL_QueryTexture(pTextureFlag, NULL, NULL, &flagRect.w, &flagRect.h);

    playerRect1.w /= 20;
    playerRect1.h /= 20;
    playerRect2.w /= 20;
    playerRect2.h /= 20;
    flagRect.w /= 5;

    //Start positions
    int player1X = playerRect1.w;
    int player1Y = playerRect1.h; 

    int player2X = WINDOW_WIDTH - playerRect2.w; 
    int player2Y = WINDOW_HEIGHT - playerRect2.h;

    flagRect.x = (WINDOW_WIDTH - flagRect.w) / 2;
    flagRect.y = (WINDOW_HEIGHT - flagRect.h) / 2;

    int player1VelocityX = 0;
    int player1VelocityY = 0;
    int player2VelocityX = 0;
    int player2VelocityY = 0;

    int flagFrame = 0;

    bool closeWindow = false;
    bool up1, down1, left1, right1;
    bool up2, down2, left2, right2;


    printf("Aloha amigos como estas\n");
    while (!closeWindow) {
        SDL_Event event;
        //printf("Hellow");
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    closeWindow = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            up1 = true;
                            break;
                        case SDL_SCANCODE_A:
                            left1 = true;
                            break;
                        case SDL_SCANCODE_S:
                            down1 = true;
                            break;
                        case SDL_SCANCODE_D:
                            right1 = true;
                            break;
                        case SDL_SCANCODE_UP:
                            up2 = true;
                            break;
                        case SDL_SCANCODE_LEFT:
                            left2 = true;
                            break;
                        case SDL_SCANCODE_DOWN:
                            down2 = true;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            right2 = true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            up1 = false;
                            break;
                        case SDL_SCANCODE_A:
                            left1 = false;
                            break;
                        case SDL_SCANCODE_S:
                            down1 = false;
                            break;
                        case SDL_SCANCODE_D:
                            right1 = false;
                            break;
                        case SDL_SCANCODE_UP:
                            up2 = false;
                            break;
                        case SDL_SCANCODE_LEFT:
                            left2 = false;
                            break;
                        case SDL_SCANCODE_DOWN:
                            down2 = false;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            right2 = false;
                            break;
                    }
                    break;
            }
        }

    SDL_RenderClear(pRenderer);

    //rendering Grid Map
    renderGridMap(pRenderer, &map, gridTexture);

    // Update flag animation frame
    SDL_Rect srcRect = { flagFrame * flagRect.w, 0, flagRect.w, flagRect.h };
    SDL_RenderCopy(pRenderer, pTextureFlag, &srcRect, &flagRect);

    moveFlag(&flagRect, pPlayer->playerX, pPlayer->playerY, player2X, player2Y, CLOSE_DISTANCE_THRESHOLD, FLAG_SPEED);

    flagFrame = (flagFrame + 1) % 5;

    // Handle player input and movement for player 1
    handlePlayerInput(&pPlayer->playerRect, &pPlayer->playerX, &pPlayer->playerY, &pPlayer->playervelocityX, &pPlayer->playervelocityY, up1, down1, left1, right1, WINDOW_WIDTH, WINDOW_HEIGHT, pPlayer->playerRect.w, pPlayer->playerRect.h, SPEED);

    // Handle player input and movement for player 2
    handlePlayerInput(&playerRect2, &player2X, &player2Y, &player2VelocityX, &player2VelocityY, up2, down2, left2, right2, WINDOW_WIDTH, WINDOW_HEIGHT, playerRect2.w, playerRect2.h, SPEED);

    // Render players
    renderPlayer(pPlayer,pRenderer);
    SDL_RenderCopy(pRenderer, pTexture2, NULL, &playerRect2);

    //printf("playerX: %d, playerY: %d\n", pPlayer->playerX, pPlayer->playerY);
    getPlayerGridPosition(pPlayer->playerX, pPlayer->playerY, &pPlayer->playerGridX, &pPlayer->playerGridY);

    // Present renderer
    SDL_RenderPresent(pRenderer);

    // Delay for frame rate control
    SDL_Delay(1000 / PLAYER_FRAME_RATE);
}
    destroyPlayer(pPlayer);
    SDL_DestroyTexture(pTexture1);
    SDL_DestroyTexture(pTexture2);
    SDL_DestroyTexture(pTextureFlag);
    SDL_DestroyTexture(gridTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    printf("Adios amigos\n");
    SDL_Quit();
    return 0;
}


*/

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../objects/player.h"
#include "../objects/flag.h"
#include "../objects/gridmap.h"
#include "../objects/server.h"
#include "../objects/client.h"

#define SPEED 100
#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

#define CLOSE_DISTANCE_THRESHOLD 10
#define FLAG_SPEED 2

typedef struct
{
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Surface surface;
    char tag;
} GameObject;

// Function prototypes
bool initSDL(SDL_Window **pWindow, SDL_Renderer **pRenderer);
void closeSDL(SDL_Window *pWindow, SDL_Renderer *pRenderer);
bool loadResources(SDL_Renderer *pRenderer, SDL_Texture **pTexture1, SDL_Texture **pTexture2, SDL_Texture **pTextureFlag);
void handleEvents(bool *closeWindow, bool up1, bool down1, bool left1, bool right1, bool up2, bool down2, bool left2, bool right2);
void updateGame(Player *pPlayer, int *player2X, int *player2Y, SDL_Rect *flagRect);
void render(SDL_Renderer *pRenderer, GridMap *map, SDL_Texture *gridTexture, Player *pPlayer, int player2X, int player2Y, SDL_Texture *pTexture2, SDL_Rect flagRect, SDL_Texture *pTextureFlag);
void cleanup(Player *pPlayer, SDL_Texture *pTexture1, SDL_Texture *pTexture2, SDL_Texture *pTextureFlag, SDL_Texture *gridTexture);

int main(int argc, char **argv)
{
    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;
    SDL_Texture *pTexture1 = NULL;
    SDL_Texture *pTexture2 = NULL;
    SDL_Texture *pTextureFlag = NULL;
    SDL_Texture *gridTexture = NULL;
    Player *pPlayer = NULL;
    SDL_Rect flagRect;
    SDL_Rect playerRect2;
    playerRect2.w /= 20;
    playerRect2.h /= 20;
    GridMap map;

    bool up1, down1, left1, right1;
    bool up2, down2, left2, right2;
    bool closeWindow = false;
    bool isServer = false;

    int player2X = WINDOW_WIDTH - playerRect2.w; 
    int player2Y = WINDOW_HEIGHT - playerRect2.h;
    int player2VelocityX = 0;
    int player2VelocityY = 0;

    // Control if the first instance of the application is the server
    if (argc > 1 && strcmp(argv[1], "server") == 0)
    {
        isServer = true;
    }

    // Initialize SDL
    if (!initSDL(&pWindow, &pRenderer))
    {
        return 1;
    }

    // Load resources
    if (!loadResources(pRenderer, &pTexture1, &pTexture2, &pTextureFlag))
    {
        closeSDL(pWindow, pRenderer);
        return 1;
    }

    // Create player based on whether it's server or client
    if (isServer)
    {
        // For server, create and listen for connections
        Server server = createServer();
        if (!server.serverSocket)
        {
            printf("Failed to create server\n");
            closeSDL(pWindow, pRenderer);
            return 1;
        }

        if (acceptClientConnections(&server) != 0)
        {
            printf("Failed to accept client connections\n");
            closeSDL(pWindow, pRenderer);
            return 1;
        }

        // Server is running, no need to create a player
    }
    else
    {
        // For client, connect to server
        Client client = createClient();
        if (connectToServer(&client) != 0)
        {
            printf("Failed to connect to server\n");
            closeClient(&client);
            closeSDL(pWindow, pRenderer);
            return 1;
        }
        // Create player for the client
        pPlayer = createPlayer(pRenderer, SPEED);
        if (!pPlayer)
        {
            printf("Failed to create player, Error: %s\n", SDL_GetError());
            closeClient(&client);
            closeSDL(pWindow, pRenderer);
            return 1;
        }
    }

    // Main game loop
    while (!closeWindow)
    {
        // Handle events
        handleEvents(&closeWindow, up1, down1, left1, right1, up2, down2, left2, right2);
        handlePlayerInput(&pPlayer->playerRect, &pPlayer->playerX, &pPlayer->playerY, &pPlayer->playervelocityX, &pPlayer->playervelocityY, up1, down1, left1, right1, WINDOW_WIDTH, WINDOW_HEIGHT, pPlayer->playerRect.w, pPlayer->playerRect.h, SPEED);
        handlePlayerInput(&playerRect2, &player2X, &player2Y, &player2VelocityX, &player2VelocityY, up2, down2, left2, right2, WINDOW_WIDTH, WINDOW_HEIGHT, playerRect2.w, playerRect2.h, SPEED);


        // Update game state
        updateGame(pPlayer, &player2X, &player2Y, &flagRect);

        // Render the game
        render(pRenderer, &map, gridTexture, pPlayer, player2X, player2Y, pTexture2, flagRect, pTextureFlag);
    }

    // Cleanup resources
    cleanup(pPlayer, pTexture1, pTexture2, pTextureFlag, gridTexture);

    // Close SDL
    closeSDL(pWindow, pRenderer);

    return 0;
}

// Initialization Functions
bool initSDL(SDL_Window **pWindow, SDL_Renderer **pRenderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }
    *pWindow = SDL_CreateWindow("CTF Gaming", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!(*pWindow))
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    *pRenderer = SDL_CreateRenderer(*pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!(*pRenderer))
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*pWindow);
        SDL_Quit();
        return false;
    }
    return true;
}

void closeSDL(SDL_Window *pWindow, SDL_Renderer *pRenderer)
{
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}

bool loadResources(SDL_Renderer *pRenderer, SDL_Texture **pTexture1, SDL_Texture **pTexture2, SDL_Texture **pTextureFlag)
{
    SDL_Surface *pSurface1 = IMG_Load("resources/player1.png");
    SDL_Surface *pSurface2 = IMG_Load("resources/Ship.png");
    SDL_Surface *pSurfaceFlag = IMG_Load("resources/flag.png");
    if (!pSurface1 || !pSurface2 || !pSurfaceFlag)
    {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }
    *pTexture1 = SDL_CreateTextureFromSurface(pRenderer, pSurface1);
    *pTexture2 = SDL_CreateTextureFromSurface(pRenderer, pSurface2);
    *pTextureFlag = SDL_CreateTextureFromSurface(pRenderer, pSurfaceFlag);
    SDL_FreeSurface(pSurface1);
    SDL_FreeSurface(pSurface2);
    SDL_FreeSurface(pSurfaceFlag);
    if (!(*pTexture1) || !(*pTexture2) || !(*pTextureFlag))
    {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

// Event Handling Function
void handleEvents(bool *closeWindow, bool up1, bool down1, bool left1, bool right1, bool up2, bool down2, bool left2, bool right2)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            *closeWindow = true;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
                up1 = true;
                break;
            case SDL_SCANCODE_A:
                left1 = true;
                break;
            case SDL_SCANCODE_S:
                down1 = true;
                break;
            case SDL_SCANCODE_D:
                right1 = true;
                break;
            case SDL_SCANCODE_UP:
                up2 = true;
                break;
            case SDL_SCANCODE_LEFT:
                left2 = true;
                break;
            case SDL_SCANCODE_DOWN:
                down2 = true;
                break;
            case SDL_SCANCODE_RIGHT:
                right2 = true;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
                up1 = false;
                break;
            case SDL_SCANCODE_A:
                left1 = false;
                break;
            case SDL_SCANCODE_S:
                down1 = false;
                break;
            case SDL_SCANCODE_D:
                right1 = false;
                break;
            case SDL_SCANCODE_UP:
                up2 = false;
                break;
            case SDL_SCANCODE_LEFT:
                left2 = false;
                break;
            case SDL_SCANCODE_DOWN:
                down2 = false;
                break;
            case SDL_SCANCODE_RIGHT:
                right2 = false;
                break;
            }
            break;
        }
    }
}

// Game Logic Function
void updateGame(Player *pPlayer, int *player2X, int *player2Y, SDL_Rect *flagRect){
    // Update player and flag positions, handle collisions, etc.
    // You need to implement this function according to your game logic
    // For demonstration purposes, here's a simple update logic for player and flag positions:

    // Update player position based on velocity
    pPlayer->playerX += pPlayer->playervelocityX / PLAYER_FRAME_RATE;
    pPlayer->playerY += pPlayer->playervelocityY / PLAYER_FRAME_RATE;

    // Update flag position (example)
    flagRect->x += 1; // Move flag to the right by 1 pixel each frame
}

// Rendering Function
void render(SDL_Renderer *pRenderer, GridMap *map, SDL_Texture *gridTexture, Player *pPlayer, int player2X, int player2Y, SDL_Texture *pTexture2, SDL_Rect flagRect, SDL_Texture *pTextureFlag)
{
    // Render grid, players, flag, etc.
    // You need to implement this function according to your rendering needs
    // For demonstration purposes, here's a simple rendering logic:
    int flagFrame = 0;

    // Clear renderer
    SDL_RenderClear(pRenderer);

    // Render grid map
    renderGridMap(pRenderer, map, gridTexture);

    // Render player 1
    renderPlayer(pPlayer, pRenderer);

    // Render player 2
    SDL_Rect player2Rect = {player2X, player2Y /*player2 width,*/ /*player2 height*/};
    SDL_RenderCopy(pRenderer, pTexture2, NULL, &player2Rect);

    // Render flag
    SDL_Rect srcRect = {flagFrame * flagRect.w, 0, flagRect.w, flagRect.h};
    SDL_RenderCopy(pRenderer, pTextureFlag, &srcRect, &flagRect);

    moveFlag(&flagRect, pPlayer->playerX, pPlayer->playerY, player2X, player2Y, CLOSE_DISTANCE_THRESHOLD, FLAG_SPEED);

    flagFrame = (flagFrame + 1) % 5;

    // Present renderer
    SDL_RenderPresent(pRenderer);
}

// Cleanup Function
void cleanup(Player *pPlayer, SDL_Texture *pTexture1, SDL_Texture *pTexture2, SDL_Texture *pTextureFlag, SDL_Texture *gridTexture)
{
    destroyPlayer(pPlayer);
    SDL_DestroyTexture(pTexture1);
    SDL_DestroyTexture(pTexture2);
    SDL_DestroyTexture(pTextureFlag);
    SDL_DestroyTexture(gridTexture);
}
