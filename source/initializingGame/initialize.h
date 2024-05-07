#ifndef INITIALIZING_H
#define INITIALIZING_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>


#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800

#define FLAG_FRAME_RATE 10
#define PLAYER_FRAME_RATE 60

#define CLOSE_DISTANCE_THRESHOLD 10
#define FLAG_SPEED 2

#define MAX_PLAYERS 6

struct game{

    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    // Player *pPlayers[MAX_PLAYERS];
    int nr_of_players;
    //TTF_Font *pFont, *pScoreFont;

	//UDPsocket pSocket;
	//UDPpacket *pPacket;
    //IPaddress clients[MAX_PLAYERS];
    int nrOfClients;
    

    bool up1, down1, left1, right1;
    bool up2, down2, left2, right2;
};
typedef struct game Game;


struct gameResources{

    // GridMap map;

};
typedef struct gameResources gameRSS;

int initGame(Game *pGame);




#endif