#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdbool.h>
#include "definitions.h"

//TCP

int connectToServer(Game *pGame);
int initTCPConnection(Game *pGame);
void checkTCP(Game *pGame);
PlayerNet *createClient(char *serverIP, int port, int id, int x, int y);
void *scanForGamesOnLocalNetwork(void *arg);
void *scanForGamesFromSavedList(void *arg);

//UDP

void sendData(Game *pGame);
void getPlayerData(Game *pGame);


#endif