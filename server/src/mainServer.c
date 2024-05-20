#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>
#include "player.h"
#include "gridmap.h"
#include "playerData.h"

struct game{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Player* pPlayer[MAX_PLAYERS];
    int nr_of_players;
    GridMap* map;
    //TTF_Font *pFont, *pScoreFont;
    //Text *pOverText, *pStartText;
    GameState state;
	UDPsocket pSocket;
	UDPpacket *pPacket;
    IPaddress clients[MAX_PLAYERS];
    int nrOfClients;
    ServerData sData;
};
typedef struct game Game;

int initiate(Game *pGame);
void run(Game *pGame);
void close(Game *pGame);
void handleInput(Game *pGame,SDL_Event *pEvent);
void add(IPaddress address, IPaddress clients[],int *pNrOfClients);
void sendGameData(Game *pGame);
void executeCommand(Game *pGame,ClientData cData);
void setUpGame(Game *pGame);

int main(int argv, char** args){
    Game g={0};
    printf("game\n");
    if(!initiate(&g)) return 1;
    printf("Initiate\n");
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
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        TTF_Quit();
        SDL_Quit();
		return 0;
	}

    pGame->pWindow = SDL_CreateWindow("Rocket Server",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,0);
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
    printf("Basic init\n");

    //pGame->pFont = TTF_OpenFont("../lib/resources/arial.ttf", 100);
    //pGame->pScoreFont = TTF_OpenFont("../lib/resources/arial.ttf", 30);
    // if(!pGame->pFont || !pGame->pScoreFont){
    //     printf("Error: %s\n",TTF_GetError());
    //     close(pGame);
    //     return 0;
    // }

    if (!(pGame->pSocket = SDLNet_UDP_Open(2000)))
	{
		printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		close(pGame);
        return 0;
	}
    printf("Socket done\n");
	if (!(pGame->pPacket = SDLNet_AllocPacket(512)))
	{
		printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		close(pGame);
        return 0;
	}
    printf("Packet done\n");

    for(int i=0;i<MAX_PLAYERS;i++)
    {
        // This needs a bit of work, look at createRocket function how it handles player spawn
        pGame->pPlayer[i] = createPlayer(pGame->pRenderer, 40 + (i*2), 40 + (i*2));/*/createRocket(i,pGame->pRenderer,WINDOW_WIDTH,WINDOW_HEIGHT)*/
        printf("Player %d created\n", i);
    }
    pGame->nr_of_players = MAX_PLAYERS;

    pGame->map = loadMapFromFile("../lib/resources/map.txt");

    //pGame->pStars = createStars(WINDOW_WIDTH*WINDOW_HEIGHT/10000,WINDOW_WIDTH,WINDOW_HEIGHT);
    //pGame->pOverText = createText(pGame->pRenderer,238,168,65,pGame->pFont,"Game over",WINDOW_WIDTH/2,WINDOW_HEIGHT/2);
    //pGame->pStartText = createText(pGame->pRenderer,238,168,65,pGame->pScoreFont,"Waiting for clients",WINDOW_WIDTH/2,WINDOW_HEIGHT/2+100);
    for(int i=0;i<MAX_PLAYERS;i++){
        if(!pGame->pPlayer[i]){
            printf("Error: %s\n",SDL_GetError());
            close(pGame);
            return 0;
        }
    }
    // if(!pGame->pOverText || !pGame->pStartText){
    //     printf("Error: %s\n",SDL_GetError());
    //     close(pGame);
    //     return 0;
    // }
    pGame->state = START;
    pGame->nrOfClients = 0;

    
    return 1;
}

void run(Game *pGame){
    int close_requested = 0;
    SDL_Event event;
    ClientData cData;

    while(!close_requested){
        switch (pGame->state)
        {
            case ONGOING:
                sendGameData(pGame);
                while(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1){
                    memcpy(&cData, pGame->pPacket->data, sizeof(ClientData));
                    executeCommand(pGame,cData);
                }
                if(SDL_PollEvent(&event)) if(event.type==SDL_QUIT) close_requested = 1;
                for(int i=0;i<MAX_PLAYERS;i++)
                    // updateRocket(pGame->pPlayer[i]);
                for(int i=0;i<MAX_PLAYERS;i++)
                    for(int j=0;j<MAX_PLAYERS;j++)
                        // if(i!=j && collideRocket(pGame->pPlayer[i],pGame->pPlayer[j])){
                        //     pGame->nr_of_players-=2;
                        //     if(pGame->nr_of_players<=1) pGame->state = GAME_OVER;
                        // }
                for(int i=0;i<MAX_PLAYERS;i++)
                    for(int j=0;j<MAX_PLAYERS;j++)
                        // if(i!=j && hitRocket(pGame->pPlayer[i],pGame->pPlayer[j])){
                        //     (pGame->nr_of_players)--;
                        //     if(pGame->nr_of_players<=1) pGame->state = GAME_OVER;
                        // } 
                SDL_SetRenderDrawColor(pGame->pRenderer,255,0,0,255);
                SDL_RenderClear(pGame->pRenderer);
                SDL_SetRenderDrawColor(pGame->pRenderer,230,230,230,255);
                //drawStars(pGame->pStars,pGame->pRenderer);
                for(int i=0;i<MAX_PLAYERS;i++)
                {
                    renderPlayer(pGame->pPlayer[i], pGame->pRenderer);
                    // drawRocket(pGame->pPlayer[i]);
                }
                SDL_RenderPresent(pGame->pRenderer);
                
                break;
            case GAME_OVER:
                //drawText(pGame->pOverText);
                sendGameData(pGame);
                if(pGame->nrOfClients==MAX_PLAYERS) pGame->nrOfClients = 0;
            case START:
                //drawText(pGame->pStartText);
                SDL_RenderPresent(pGame->pRenderer);
                if(SDL_PollEvent(&event) && event.type==SDL_QUIT) close_requested = 1;
                //printf("Waiting for packets, nrofclients: %d, nrofplayers: %d\n", pGame->nrOfClients, pGame->nr_of_players);
                if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==1)
                {
                    add(pGame->pPacket->address,pGame->clients,&(pGame->nrOfClients));
                    if(pGame->nrOfClients==MAX_PLAYERS) 
                    {
                        printf("Max players reached\n");
                        setUpGame(pGame);
                    }
                    printf("Packet recieved\n");
                }
                else if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==0)
                {
                    //printf("Listening for packets\n");
                }
                else if(SDLNet_UDP_Recv(pGame->pSocket,pGame->pPacket)==-1)
                {
                    printf("-1\n");
                }
                else
                {
                    printf("Lower than -1?\n");
                }
                break;
        }
        //SDL_Delay(1000/60-15);//might work when you run on different processors
    }
}

void setUpGame(Game *pGame){
    // for(int i=0;i<MAX_PLAYERS;i++) resetRocket(pGame->pPlayer[i]);
    pGame->nr_of_players=MAX_PLAYERS;
    pGame->state = ONGOING;
}

void sendGameData(Game *pGame){
    pGame->sData.gState = pGame->state;
    for(int i=0;i<MAX_PLAYERS;i++){
        // printf("trying to send player data\n");
        getPlayerSendData(pGame->pPlayer[i], &(pGame->sData.players[i]));
    }
    for(int i=0;i<MAX_PLAYERS;i++){
        //printf("Whatever the fuck this is doing, %d\n", i);
        pGame->sData.playerNr = i;
        memcpy(pGame->pPacket->data, &(pGame->sData), sizeof(ServerData));
		pGame->pPacket->len = sizeof(ServerData);
        pGame->pPacket->address = pGame->clients[i];
		SDLNet_UDP_Send(pGame->pSocket,-1,pGame->pPacket);
    }
}

void add(IPaddress address, IPaddress clients[],int *pNrOfClients){
	for(int i=0;i<*pNrOfClients;i++) if(address.host==clients[i].host &&address.port==clients[i].port) return;
	clients[*pNrOfClients] = address;
	(*pNrOfClients)++;
}

void executeCommand(Game *pGame,ClientData cData){
    // switch (cData.command)
    // {
    //     case ACC:
    //         accelerate(pGame->pPlayer[cData.playerNumber]);
    //         break;
    //     case LEFT:
    //         turnLeft(pGame->pPlayer[cData.playerNumber]);
    //         break;
    //     case RIGHT:
    //         turnRight(pGame->pPlayer[cData.playerNumber]);
    //         break;
    //     case FIRE:
    //         fireRocket(pGame->pPlayer[cData.playerNumber]);
    //         break;
    // }
}

void close(Game *pGame){
    for(int i=0;i<MAX_PLAYERS;i++) if(pGame->pPlayer[i]) destroyPlayer(pGame->pPlayer[i]);
    //if(pGame->pStars) destroyStars(pGame->pStars);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);

    //if(pGame->pOverText) destroyText(pGame->pOverText);
    //if(pGame->pStartText) destroyText(pGame->pStartText);   
    // if(pGame->pFont) TTF_CloseFont(pGame->pFont);
    // if(pGame->pScoreFont) TTF_CloseFont(pGame->pScoreFont);

    if(pGame->pPacket) SDLNet_FreePacket(pGame->pPacket);
	if(pGame->pSocket) SDLNet_UDP_Close(pGame->pSocket);

    SDLNet_Quit();
    TTF_Quit();    
    SDL_Quit();
}