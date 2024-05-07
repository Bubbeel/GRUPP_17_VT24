/*
//UNDER CONSTRUCTION TO BE IMPLEMENTED IN THE FUTURE
#include "../objects/game.h"
#include "../objects/player.h"
#include "../objects/gridmap.h"

void initSDL();
void cleanupSDL();
void handleEvents(Player *player1, SDL_Rect *player2Rect, int *up1, int *down1, int *left1, int *right1, int *up2, int *down2, int *left2, int *right2);
void render(Player *player1, SDL_Rect *player2Rect, SDL_Texture *texturePlayer1, SDL_Texture *texturePlayer2, SDL_Renderer *renderer);
void destroy(Player *player1, SDL_Texture *texturePlayer1, SDL_Texture *texturePlayer2, SDL_Renderer *renderer);

void runGame() {
    initSDL();

    Player *player1 = createPlayer();
    SDL_Rect player2Rect;

    SDL_Window *window = SDL_CreateWindow("CTF Gaming", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    bool closeWindow = false;
    int up1, down1, left1, right1;
    int up2, down2, left2, right2;

    while (!closeWindow) {
        handleEvents(player1, &player2Rect, &up1, &down1, &left1, &right1, &up2, &down2, &left2, &right2);
        render(player1, &player2Rect, texturePlayer1, texturePlayer2, renderer);
    }

    destroy(player1, texturePlayer1, texturePlayer2, renderer);
    cleanupSDL();
}

void initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s\n", SDL_GetError());
        exit(1);
    }
}

void cleanupSDL() {
    SDL_Quit();
}

void handleEvents(Player *player1, SDL_Rect *player2Rect, int *up1, int *down1, int *left1, int *right1, int *up2, int *down2, int *left2, int *right2) {
    // event handling
}

void render(Player *player1, SDL_Rect *player2Rect, SDL_Texture *texturePlayer1, SDL_Texture *texturePlayer2, SDL_Renderer *renderer) {
    // renderlogic
}

void destroy(Player *player1, SDL_Texture *texturePlayer1, SDL_Texture *texturePlayer2, SDL_Renderer *renderer) {
    // release resources and textures
}

*/ 
