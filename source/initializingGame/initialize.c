#include "initialize.h"

//SDL_Window **pWindow, SDL_Renderer **pRenderer

int initGame(Game *pGame)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return 0;
    }
    pGame->pWindow = SDL_CreateWindow("CTF Gaming", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!(pGame->pWindow))
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!(pGame->pRenderer))
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pGame->pWindow);
        SDL_Quit();
        return 0;
    }
    /*
    if (SDLNet_Init() == -1)
	{
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
        SDL_DestroyRenderer(pGame->pRenderer);
        SDL_DestroyWindow(pGame->pWindow);
        SDL_Quit();
		return 0;
	}
    */

    return 1;
}