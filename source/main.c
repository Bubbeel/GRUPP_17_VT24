#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../objects/player.h"
#include "../objects/flag.h"
#include "../objects/gridmap.h"
#include "../objects/serverClient.h"
#include "../objects/collisionDetection.h"
#include "../objects/menu.h"
#include "../objects/common.h"

#define SPEED 100
#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

#define CLOSE_DISTANCE_THRESHOLD 10
#define FLAG_SPEED 2
#define MAX_CLIENTS 4

bool initSDL(SDL_Window **pWindow, SDL_Renderer **pRenderer);
void closeSDL(SDL_Window *pWindow, SDL_Renderer *pRenderer);
bool loadResources(SDL_Renderer *pRenderer, SDL_Texture **pTexture1, SDL_Texture **pTextureFlag);
void handleEvents(bool *closeWindow, PlayerMovementData *movementData, Client *pClient, Server *pServer);
void updateGameData(Client *pClients, int *pNumClients, Player *pPlayer, Server *pServer);
void render(SDL_Renderer *pRenderer, GridMap *map, SDL_Texture *gridTexture, Player *pPlayer, SDL_Texture *pTexture1, SDL_Rect flagRect, SDL_Texture *pTextureFlag, Flag* flag, Client *pClient, Server *pServer);
void cleanup(Player *pPlayer, SDL_Texture *pTexture1, SDL_Texture *pTextureFlag, SDL_Texture *gridTexture);
void renderOtherClients(SDL_Renderer* pRenderer, Client* clients, SDL_Texture* clientTexture, Server *pServer, Player *pPlayer);

int main(int argc, char **argv)
{
     // Initialize SDL
    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;
    if (!initSDL(&pWindow, &pRenderer))
    {
        return 1;
    }

    // Load textures
    SDL_Texture *pTexture1 = NULL;
    SDL_Texture *pTextureFlag = NULL;
    SDL_Texture *gridTexture = NULL;
    if (!loadResources(pRenderer, &pTexture1, &pTextureFlag))
    {
        closeSDL(pWindow, pRenderer);
        return 1;
    }

    // Initialize game objects
    Flag* flag=malloc(sizeof(Flag));
    Player *pPlayer=malloc(sizeof(Player));
    
    GridMap map;
    SDL_Rect flagRect;

    // Set up variables
    bool closeWindow = false;
    PlayerMovementData *movementData=malloc(sizeof(PlayerMovementData));
    Server *pServer = NULL;
    Client *pClient = NULL;
    int numClients = 0;
    /*
    flagRect.w /= 5;
    flagRect.x = WINDOW_WIDTH / 2;
    flagRect.y = WINDOW_HEIGHT / 2;
    flag->flagX = flag->flagRect.x;
    flag->flagY = flag->flagRect.y;*/

    // Load resources

    if (argc > 1 && strcmp(argv[1], "server") == 0)
    {
        // Create server instance
        pServer = createServer();
        if (!pServer)
        {
            printf("Failed to create server\n");
            closeSDL(pWindow, pRenderer);
            return 1;
        }

        // Wait for clients to connect
        while (numClients < MAX_CLIENTS)
        {
            int clientIndex = waitForClients(pServer);
            if (clientIndex < 0)
            {
                printf("Failed to accept client.\n");
                continue;
            }

            // Handle the client connection
            GameObject gameObject; // Initialize this as needed
            numClients++;
        }
    }
    else
    {
        // Create client instance
        pClient = createClient();
        if (!pClient)
        {
            printf("Failed to create client\n");
            closeSDL(pWindow, pRenderer);
            return 1;
        }

        // Connect to server
        if (!pClient)
        {
            printf("Failed to connect to server\n");
            closeSDL(pWindow, pRenderer);
            return 1;
        }
    }
    while (!closeWindow&&numClients>=1)
    {
        handleEvents(&closeWindow, movementData, pClient, pServer);

        // Update game data
        updateGame(pServer, pClient, pPlayer);

        // Render
        render(pRenderer, &map, gridTexture, pPlayer, pTexture1, flagRect, pTextureFlag, flag, pClient, pServer);

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
    if (SDLNet_Init() == -1)
    {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        SDL_DestroyRenderer(*pRenderer);
        SDL_DestroyWindow(*pWindow);
        SDL_Quit();
        return false;
    }
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
    GameObject *gameObject=malloc(sizeof(GameObject));
    gameObject->renderer = *pRenderer;
    gameObject->tag = 'p';
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

    if (!pRenderer) {
        printf("Error: Renderer is null\n");
        return false;
    }

    if (!pTexture1 || !pTextureFlag) {
        printf("Error: Texture pointer is null\n");
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

void handleEvents(bool *closeWindow, PlayerMovementData *movementData, Client *pClient, Server *pServer) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *closeWindow = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        movementData->up = true;
                        break;
                    case SDL_SCANCODE_A:
                        movementData->left = true;
                        break;
                    case SDL_SCANCODE_S:
                        movementData->down = true;
                        break;
                    case SDL_SCANCODE_D:
                        movementData->right = true;
                        break;
                }
                sendPlayerPosition(pClient, movementData, pServer);
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        movementData->up = false;
                        break;
                    case SDL_SCANCODE_A:
                        movementData->left = false;
                        break;
                    case SDL_SCANCODE_S:
                        movementData->down = false;
                        break;
                    case SDL_SCANCODE_D:
                        movementData->right = false;
                        break;
                }
                sendPlayerPosition(pClient, movementData, pServer);
                break;
        }
    }
}

void render(SDL_Renderer *pRenderer, GridMap *map, SDL_Texture *gridTexture, Player *pPlayer, SDL_Texture *pTexture1, SDL_Rect flagRect, SDL_Texture *pTextureFlag, Flag* flag, Client *pClient, Server *pServer)
{
    int flagFrame=0;
 
    SDL_RenderClear(pRenderer);

    renderGridMap(pRenderer, map, gridTexture);
    renderOtherClients(pRenderer, pClient, pTexture1, pServer, pPlayer);
    SDL_Rect srcRect = {flagFrame * flagRect.w, flagRect.w, flagRect.h};

    SDL_RenderCopy(pRenderer, pTextureFlag, NULL, &flag->flagRect);
    //moveFlag(&flagRect, pPlayer->playerX, pPlayer->playerY, CLOSE_DISTANCE_THRESHOLD, FLAG_SPEED);

    flagFrame = (flagFrame + 1) % 5;

    SDL_RenderPresent(pRenderer);
}


void cleanup(Player *pPlayer, SDL_Texture *pTexture1, SDL_Texture *pTextureFlag, SDL_Texture *gridTexture)
{
    destroyPlayer(pPlayer);
    SDL_DestroyTexture(pTexture1);
    SDL_DestroyTexture(pTextureFlag);
    SDL_DestroyTexture(gridTexture);
}

void renderOtherClients(SDL_Renderer* pRenderer, Client* clients, SDL_Texture* clientTexture, Server *pServer, Player *pPlayer) {
    for (int i = 0; i < 4; i++) {
        // Check if the client has a valid player
        if (clients->players[i] != NULL) {
            // Render the player of the client
            renderPlayer(clients->players[i], pRenderer);

        }else{
            printf("Client %d has no player\n", i);
        }
    }
}
