#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../objects/player.h"
#include "../objects/flag.h"
#include "../objects/gridMap.h"
#include "../objects/server.h"
#include "../objects/client.h"
#include "../objects/collisionDetection.h"
#include "../objects/common.h"


#define SPEED 100
#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

#define CLOSE_DISTANCE_THRESHOLD 10
#define FLAG_SPEED 2

bool initSDL(SDL_Window **pWindow, SDL_Renderer **pRenderer);
void closeSDL(SDL_Window *pWindow, SDL_Renderer *pRenderer);
bool loadResources(SDL_Renderer *pRenderer, SDL_Texture **pTexture1, SDL_Texture **pTextureFlag);
void handleEvents(bool *closeWindow, bool *up1, bool *down1, bool *left1, bool *right1);
void updateGame(SDL_Renderer* pRenderer, Player *pPlayer,PlayerPackage *pPlayerPackage, SDL_Rect *flagRect, Client *pClient);
void render(SDL_Renderer *pRenderer, GridMap *map, SDL_Texture *gridTexture, Player *pPlayer, SDL_Texture *pTexture1, SDL_Rect flagRect, SDL_Texture *pTextureFlag, Flag* flag, Client *pClient);
void cleanup(Player *pPlayer, SDL_Texture *pTexture1, SDL_Texture *pTextureFlag, SDL_Texture *gridTexture);

int main(int argc, char **argv)
{
    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;
    if (!initSDL(&pWindow, &pRenderer))
    {
        return 1;
    }
    SDL_Texture *pTexture1 = NULL;
    SDL_Texture *pTextureFlag = NULL;
    SDL_Texture *gridTexture = NULL;
    Client *pClient = createClient();
    Server *pServer = createServer();
    Player *pPlayer = createPlayer(pRenderer, SPEED);
    Flag* flag = createFlag(pRenderer);
    PlayerPackage *pPlayerPackage;
    SDL_Rect flagRect;
    SDL_Rect playerRect1;
    GridMap map;

    bool up1, down1, left1, right1;
    bool closeWindow = false;
    bool isServer = false;

    int player1X = playerRect1.w;
    int player1Y = playerRect1.h; 
    int player1VelocityX = 0;
    int player1VelocityY = 0;
    /*flagRect.w /= 5;
    flagRect.x = WINDOW_WIDTH / 2;
    flagRect.y = WINDOW_HEIGHT / 2;
    flag->flagX = flag->flagRect.x;
    flag->flagY = flag->flagRect.y;*/

 
    // Control if the first instance of the application is the server
    if (argc > 1 && strcmp(argv[1], "server") == 0)
    {
        isServer = true;
    }

    // Load resources
    if (!loadResources(pRenderer, &pTexture1, &pTextureFlag))
    {
        closeSDL(pWindow, pRenderer);
        return 1;
    }

    // Create player based on whether it's server or client
    if (isServer)
    {
        // For server, create and listen for connections
        if (pServer == NULL) {
            printf("Failed to allocate memory for server\n");
            return 1;
        }
        if (!pServer->serverSocket)
        {
            printf("Failed to create server\n");
            closeSDL(pWindow, pRenderer);
            return 1;
        }

        if (acceptClientConnections(pServer) != 0)
        {
            printf("Failed to accept client connections\n");
            closeSDL(pWindow, pRenderer);
            return 1;
        }

    }
    else
    {
        // For client, connect to server
        if(pClient == NULL){
            printf("Failed to allocate memory for client\n");
            return 1;
        }
        *pClient = *createClient();
        if (connectToServer(pClient) != 0)
        {
            printf("Failed to connect to server\n");
            closeClient(pClient);
            closeSDL(pWindow, pRenderer);
            return 1;
        }
        // Create player for the client
        pPlayer = createPlayer(pRenderer, SPEED);
        if (!pPlayer)
        {
            printf("Failed to create player, Error: %s\n", SDL_GetError());
            closeClient(pClient);
            closeSDL(pWindow, pRenderer);
            return 1;
        }
    }

    // Main game loop
    while (!closeWindow)
    {
        // Handle events
        handleEvents(&closeWindow, &up1, &down1, &left1, &right1);
        // Update game state
        updateGame(pRenderer, pPlayer,  pPlayerPackage, &flagRect, pClient);
        handlePlayerInput(&pPlayer->playerRect, &pPlayer->playerX, &pPlayer->playerY, &pPlayer->playervelocityX, &pPlayer->playervelocityY, up1, down1, left1, right1, WINDOW_WIDTH, WINDOW_HEIGHT, pPlayer->playerRect.w, pPlayer->playerRect.h, SPEED);
        // Render the game
        render(pRenderer, &map, gridTexture, pPlayer,pTexture1, flagRect, pTextureFlag, flag, pClient);
        if (isServer) {
            listenForClientData(pServer, pClient, pPlayer); 
            sendGameData(pServer, pClient, pPlayer);
        }
        else {
            sendDataUDP(pClient, pPlayer);
        }

    }

    // Cleanup resources
    cleanup(pPlayer, pTexture1, pTextureFlag, gridTexture);

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

bool loadResources(SDL_Renderer *pRenderer, SDL_Texture **pTexture1, SDL_Texture **pTextureFlag)
{
    SDL_Surface *pSurface1 = IMG_Load("resources/player1.png");
    SDL_Surface *pSurfaceFlag = IMG_Load("resources/flag.png");
    if (!pSurface1 || !pSurfaceFlag)
    {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }
    *pTexture1 = SDL_CreateTextureFromSurface(pRenderer, pSurface1);
    *pTextureFlag = SDL_CreateTextureFromSurface(pRenderer, pSurfaceFlag);
    SDL_FreeSurface(pSurface1);
    SDL_FreeSurface(pSurfaceFlag);
    if (!(*pTexture1) || !(*pTextureFlag))
    {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

// Event Handling Function
void handleEvents(bool *closeWindow, bool *up1, bool *down1, bool *left1, bool *right1)
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
                *up1 = true;
                break;
            case SDL_SCANCODE_A:
                *left1 = true;
                break;
            case SDL_SCANCODE_S:
                *down1 = true;
                break;
            case SDL_SCANCODE_D:
                *right1 = true;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
                *up1 = false;
                break;
            case SDL_SCANCODE_A:
                *left1 = false;
                break;
            case SDL_SCANCODE_S:
                *down1 = false;
                break;
            case SDL_SCANCODE_D:
                *right1 = false;
                break;
            }
            break;
        }
    }
}
void updateGame(SDL_Renderer* pRenderer, Player *pPlayer,PlayerPackage *pPlayerPackage, SDL_Rect *flagRect, Client *pClient){
    pPlayer->playerX += pPlayer->playervelocityX / PLAYER_FRAME_RATE;
    pPlayer->playerY += pPlayer->playervelocityY / PLAYER_FRAME_RATE;

    flagRect->x += FLAG_SPEED;

    PlayerPackage *pkg;

    if (receiveFromServer(pClient, pPlayer) == 0)
    {
        pPlayer->playerX = pkg->x;
        pPlayer->playerY = pkg->y;
        pPlayer->direction = pkg->direction;
        printf("Received player data: X=%d, Y=%d, Direction=%d\n", pPlayer->playerX, pPlayer->playerY, pPlayer->direction);
    }
    else
    {
        fprintf(stderr, "No data received from server\n");
    }

}

// Rendering Function
void render(SDL_Renderer *pRenderer, GridMap *map, SDL_Texture *gridTexture, Player *pPlayer, SDL_Texture *pTexture1, SDL_Rect flagRect, SDL_Texture *pTextureFlag, Flag* flag, Client *pClient)
{
    // Render grid, players, flag, etc.
    // You need to implement this function according to your rendering needs
    // For demonstration purposes, here's a simple rendering logic:

    // Clear renderer
    SDL_RenderClear(pRenderer);

    // Render grid map
    renderGridMap(pRenderer, map, gridTexture);

    // Render player
    renderPlayer(pPlayer, pRenderer);

    // Render flag
    SDL_RenderCopy(pRenderer, pTextureFlag, NULL, &flag->flagRect);

    // Present renderer
    SDL_RenderPresent(pRenderer);
}

// Cleanup Function
void cleanup(Player *pPlayer, SDL_Texture *pTexture1, SDL_Texture *pTextureFlag, SDL_Texture *gridTexture)
{
    destroyPlayer(pPlayer);
    SDL_DestroyTexture(pTexture1);
    SDL_DestroyTexture(pTextureFlag);
    SDL_DestroyTexture(gridTexture);
}
