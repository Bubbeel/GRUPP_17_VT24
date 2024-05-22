#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>
#include "player.h"
#include <gridmap.h>
#include <playerData.h>

struct game{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Player* pPlayer[MAX_PLAYERS];
    int nr_of_players, playerNr;
    GridMap* map;
    //Stars *pStars;
    //TTF_Font *pFont, *pScoreFont;
    //Text *pOverText, *pStartText, *pWaitingText;
    GameState state;

    UDPsocket pSocket;
	IPaddress serverAddress;
	UDPpacket *pPacket;

};
typedef struct game Game;

int initiate(Game *pGame);
void run(Game *pGame);
void close(Game *pGame);
void handleInput(Game *pGame,SDL_Event *pEvent);
void updateWithServerData(Game *pGame);

int main(int argv, char** args){
    Game g={0};
    printf("Initiating\n");
    if(!initiate(&g)) return 1;
    printf("Initiated, running\n");
    run(&g);
    printf("Closing\n");
    close(&g);

    return 0;
}

int initiate(Game *pGame){
    srand(time(NULL));
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 0;
    }
    if(TTF_Init()!=0){
        printf("Error: %s\n",TTF_GetError());
        SDL_Quit();
        return 0;
    }
    if (SDLNet_Init())
	{
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
        TTF_Quit();
        SDL_Quit();
		return 0;
	}


    pGame->pWindow = SDL_CreateWindow("Rocket Client",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,0);
    if(!pGame->pWindow){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;
    }
    pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!pGame->pRenderer){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;    
    }

    //pGame->pFont = TTF_OpenFont("../lib/resources/arial.ttf", 100);
    //pGame->pScoreFont = TTF_OpenFont("../lib/resources/arial.ttf", 30);
    // if(!pGame->pFont || !pGame->pScoreFont){
    //     printf("Error: %s\n",TTF_GetError());
    //     close(pGame);
    //     return 0;
    // }

    printf("Before socket\n");
    if (!(pGame->pSocket = SDLNet_UDP_Open(0)))//0 means not a server
	{
		printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		return 0;
	}
    printf("Socket aquired\n");
	if (SDLNet_ResolveHost(&(pGame->serverAddress), "127.0.0.1", 2000))
	{
		printf("SDLNet_ResolveHost(127.0.0.1 2000): %s\n", SDLNet_GetError());
		return 0;
	}
    printf("Host resolved\n");
    if (!(pGame->pPacket = SDLNet_AllocPacket(512)))
	{
		printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		return 0;
	}
    printf("Packet allocated\n");
    pGame->pPacket->address.host = pGame->serverAddress.host;
    pGame->pPacket->address.port = pGame->serverAddress.port;

    pGame->map = loadMapFromFile("../lib/resources/map.txt");

    for(int i=0;i<MAX_PLAYERS;i++)
        pGame->pPlayer[i] = createPlayer(pGame->pRenderer,40,10);
        printf("Player created\n");
    pGame->nr_of_players = MAX_PLAYERS;
    for(int i=0;i<MAX_PLAYERS;i++){
        if(!pGame->pPlayer[i]){
            printf("Error: %s\n",SDL_GetError());
            close(pGame);
            return 0;
        }
    }
    pGame->state = START;

    
    return 1;
}

void run(Game *pGame){
    int close_requested = 0;
    SDL_Event event;
    ClientData cData;
    
    int joining = 0;
    while(!close_requested){
        switch (pGame->state)
        {
            case ONGOING:
                while(SDLNet_UDP_Recv(pGame->pSocket, pGame->pPacket)){
                    updateWithServerData(pGame);
                }			                    

                if(SDL_PollEvent(&event)){
                    if(event.type==SDL_QUIT) close_requested = 1;
                    else handleInput(pGame,&event);
                }
                //printf("my playernr: %d\n", pGame->playerNr);
                for(int i=0;i<MAX_PLAYERS;i++)
                    //updateRocket(pGame->pPlayer[i]);
                SDL_SetRenderDrawColor(pGame->pRenderer,0,0,0,255);
                SDL_RenderClear(pGame->pRenderer); 
                SDL_SetRenderDrawColor(pGame->pRenderer,230,230,230,255);
                renderGridMapCentered(pGame->pRenderer, pGame->map, pGame->pPlayer[pGame->playerNr], WINDOW_WIDTH, WINDOW_HEIGHT, LEVEL_WIDTH, LEVEL_HEIGHT);
                //printf("My playerNr: %d\n", pGame->playerNr);
                //drawStars(pGame->pStars,pGame->pRenderer);
                for(int i=0;i<MAX_PLAYERS;i++)
                {
                    renderPlayer(pGame->pPlayer[i], pGame->pRenderer);
                    //printf("playerNr: %d, posx: %d, posy: %d, rectx: %d, recty: %d\n",i, pGame->pPlayer[i]->playerX, pGame->pPlayer[i]->playerY, pGame->pPlayer[i]->playerRect.x, pGame->pPlayer[i]->playerRect.y);
                }
                SDL_RenderPresent(pGame->pRenderer);
                
                break;
            case GAME_OVER:
                //drawText(pGame->pOverText);
            case START:
                if(!joining){
                    //drawText(pGame->pStartText);
                }else{
                    SDL_SetRenderDrawColor(pGame->pRenderer,0,0,0,255);
                    SDL_RenderClear(pGame->pRenderer);
                    SDL_SetRenderDrawColor(pGame->pRenderer,230,230,230,255);
                    //drawText(pGame->pWaitingText);
                }
                SDL_RenderPresent(pGame->pRenderer);
                if(SDL_PollEvent(&event)){
                    if(event.type==SDL_QUIT) close_requested = 1;
                    else if(!joining && event.type==SDL_KEYDOWN && event.key.keysym.scancode==SDL_SCANCODE_SPACE){
                        joining = 1;
                        //cData.command=READY;
                        cData.playerNumber=-1;
                        memcpy(pGame->pPacket->data, &cData, sizeof(ClientData));
		                pGame->pPacket->len = sizeof(ClientData);
                    }
                }
                if(joining) SDLNet_UDP_Send(pGame->pSocket, -1,pGame->pPacket);
                if(SDLNet_UDP_Recv(pGame->pSocket, pGame->pPacket)){
                    printf("Server sent packet\n");
                    updateWithServerData(pGame);
                    if(pGame->state == ONGOING) joining = 0;
                }                
                break;
        }
        //SDL_Delay(1000/60-15);//might work when you run on different processors
    }
}

void updateWithServerData(Game *pGame){
    // printf("Updating with server data\n");
    ServerData sData;
    memcpy(&sData, pGame->pPacket->data, sizeof(ServerData));
    pGame->playerNr = sData.playerNr;
    pGame->pPlayer[pGame->playerNr]->playerNumber = pGame->playerNr;
    pGame->state = sData.gState;
    //We might need a function to get mapdata as well to update the map
    for(int i=0;i<MAX_PLAYERS;i++){
        updatePlayerWithRecievedData(pGame->pPlayer[i],&(sData.players[i]));
        // printf("player %d, pos: %d, posy: %d\n", i, pGame->pPlayer[i]->playerX, pGame->pPlayer[i]->playerY);
    }
}

void handleInput(Game *pGame,SDL_Event *pEvent){
    if(pEvent->type == SDL_KEYDOWN){
        ClientData cData;
        cData.playerNumber = pGame->playerNr;
        cData.command.DOWN = false;
        cData.command.UP = false;
        cData.command.RIGHT = false;
        cData.command.LEFT = false;
        switch(pEvent->key.keysym.scancode){
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                //accelerate(pGame->pPlayer[pGame->playerNr]);
                cData.command.UP = true;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                //turnLeft(pGame->pPlayer[pGame->playerNr]);
                cData.command.LEFT = true;
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                //turnRight(pGame->pPlayer[pGame->playerNr]);
                cData.command.RIGHT = true;
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                //go down
                cData.command.DOWN = true;
                break;
            case SDL_SCANCODE_SPACE:
                //fireRocket(pGame->pPlayer[pGame->playerNr]);
                //cData.command = FIRE;
                break;
        }
        handlePlayerInput(pGame->pPlayer[pGame->playerNr], cData, LEVEL_WIDTH, LEVEL_HEIGHT);
        //printf("px: %d, py: %d\n",  pGame->pPlayer[pGame->playerNr]->playerX,  pGame->pPlayer[pGame->playerNr]->playerY);
        cData.x = pGame->pPlayer[pGame->playerNr]->playerX;
        cData.y = pGame->pPlayer[pGame->playerNr]->playerY;
        //printf("cdatax: %d, cdatay: %d", cData.x, cData.y);
        cData.playerNumber = pGame->playerNr;
        memcpy(pGame->pPacket->data, &cData, sizeof(ClientData));
		pGame->pPacket->len = sizeof(ClientData);
        SDLNet_UDP_Send(pGame->pSocket, -1,pGame->pPacket);
    }
}

void close(Game *pGame){
    //for(int i=0;i<MAX_PLAYERS;i++) if(pGame->pPlayer[i]) destroyRocket(pGame->pPlayer[i]);
    //if(pGame->pStars) destroyStars(pGame->pStars);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);
    destroyGridMap(pGame->map);
    for(int i=0;i<MAX_PLAYERS;i++)
    {
        destroyPlayer(pGame->pPlayer[i]);
    }
    
    // if(pGame->pWaitingText) destroyText(pGame->pWaitingText);
    // if(pGame->pOverText) destroyText(pGame->pOverText);
    // if(pGame->pStartText) destroyText(pGame->pStartText);   
    // if(pGame->pFont) TTF_CloseFont(pGame->pFont);
    // if(pGame->pScoreFont) TTF_CloseFont(pGame->pScoreFont);



    SDLNet_Quit();
    TTF_Quit();    
    SDL_Quit();
}