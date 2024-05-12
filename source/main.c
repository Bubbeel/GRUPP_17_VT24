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
#include "../objects/server.h"
#include "../objects/client.h"
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

bool initSDL(SDL_Window **pWindow, SDL_Renderer **pRenderer);
void closeSDL(SDL_Window *pWindow, SDL_Renderer *pRenderer);
bool loadResources(SDL_Renderer *pRenderer, SDL_Texture **pTexture1, SDL_Texture **pTextureFlag);
void handleEvents(bool *closeWindow, PlayerMovementData *movementData, Client *pClient);
void updateGame(Client *pClients, int *pNumClients, Player *pPlayer, PlayerMovementData *movementData, Server *pServer);
void render(SDL_Renderer *pRenderer, GridMap *map, SDL_Texture *gridTexture, Player *pPlayer, SDL_Texture *pTexture1, SDL_Rect flagRect, SDL_Texture *pTextureFlag, Flag* flag, Client *pClient, Server *pServer);
void cleanup(Player *pPlayer, SDL_Texture *pTexture1, SDL_Texture *pTextureFlag, SDL_Texture *gridTexture);
void renderOtherClients(SDL_Renderer* pRenderer, Client* clients, SDL_Texture* clientTexture, Server *pServer, Player *pPlayer);
void sendPlayerPosition(Client *client, Player *player);

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
    PlayerMovementData movementData;
    Client *pClient = createClient();
    Server *pServer = createServer();
    Flag* flag = createFlag(pRenderer);
    Player *pPlayer = createPlayer(pRenderer);
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
    int *pNumClients=malloc(sizeof(int));
    *pNumClients=0;
    /*
    flagRect.w /= 5;
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
        if (acceptClientConnections(pServer, pNumClients, pClient) != 0)
        {
            printf("Failed to accept client connections\n");
            closeSDL(pWindow, pRenderer);
            return 1;
        }
    }
    else
    {
        if(pClient == NULL){
            printf("Failed to allocate memory for client\n");
            return 1;
        }
        if (connectToServer(pClient) != 0)
        {
            printf("Failed to connect to server\n");
            closeClient(pClient);
            closeSDL(pWindow, pRenderer);
            return 1;
        }
        if (!pPlayer)
        {
            printf("Failed to create player, Error: %s\n", SDL_GetError());
            closeClient(pClient);
            closeSDL(pWindow, pRenderer);
            return 1;
        }
        (*pNumClients)++;
    }
    printf("numclients:%d\n", *pNumClients);

    // Display menu and handle user choice
    /*int menuChoice = displayMenu(pWindow, pRenderer);
    if (menuChoice == 2) // If user chooses to quit from the menu
    {
        closeWindow = true;
    }*/

    // Main game loop
    while (!closeWindow)
    {
        render(pRenderer, &map, gridTexture, pPlayer,pTexture1, flagRect, pTextureFlag, flag, pClient, pServer);

        handleEvents(&closeWindow, &movementData, pClient);   

        handlePlayerInput(&pPlayer->playerRect, &pPlayer->playerX, &pPlayer->playerY, &pPlayer->playervelocityX, &pPlayer->playervelocityY, up1, down1, left1, right1, WINDOW_WIDTH, WINDOW_HEIGHT, pPlayer->playerRect.w, pPlayer->playerRect.h, SPEED);

        updateGame(pClient, pNumClients, pPlayer,  &movementData, pServer);

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
    GameObject gameObject;
    gameObject.renderer = *pRenderer;
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

// Event Handling Function
void handleEvents(bool *closeWindow, PlayerMovementData *movementData, Client *pClient) {
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
                break;
        }
    }
}

void updateGame(Client *pClients, int *pNumClients, Player *pPlayer, PlayerMovementData *movementData, Server *pServer) {
    
    printf("numclients=%d\n", *pNumClients);
    for(int i=0; i<*pNumClients; i++){
        sendDataUDP(&pServer->clients[i], pPlayer);
        listenForClientData(pServer, &pServer->clients[i], pNumClients); 
        printf("Sent player data: X=%d, Y=%d, Direction=%d\n", pPlayer->playerX, pPlayer->playerY, pPlayer->direction);
        handlePlayerInput(&pPlayer->playerRect, &pPlayer->playerX, &pPlayer->playerY, &pPlayer->playervelocityX, &pPlayer->playervelocityY, movementData->up, movementData->down, movementData->left, movementData->right, WINDOW_WIDTH, WINDOW_HEIGHT, pPlayer->playerRect.w, pPlayer->playerRect.h, SPEED);
        receiveFromServer(&pServer->clients[i], pPlayer);
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
    moveFlag(&flagRect, pPlayer->playerX, pPlayer->playerY, CLOSE_DISTANCE_THRESHOLD, FLAG_SPEED);

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
    for (int i = 0; i < 1; i++) {
        // Check if the client has a valid player
        if (clients->player[i] != NULL) {
            // Render the player of the client
            renderPlayer(clients->player[i], pRenderer);

        }else{
            printf("Client %d has no player\n", i);
        }
    }
}
