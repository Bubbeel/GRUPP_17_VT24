#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>
#include "music.h"
#include "player.h"
#include "gridmap.h"
#include "playerData.h"
#include "collisionDetection.h"
#include "weapon.h"
#include "flag.h"
#include "menu.h"
#include "text.h"


struct game{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    SDL_Texture* gridTexture;
    Player* pPlayer[MAX_PLAYERS];
    int nr_of_players;
    GridMap* map;
    Weapon* weapon[5];
    Flag* flag;
    GameUI* pointsUI;
    GameUI* currentPointsDisplayUI;
    bool holdingFlagSound;
    TTF_Font *pFont, *pScoreFont;
    Text *pOverText, *pStartText, *pWaitingText, *pYouLoseText, *pYouWonText;
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

    pGame->pFont = TTF_OpenFont("../lib/resources/arial.ttf", 100);
    pGame->pScoreFont = TTF_OpenFont("../lib/resources/arial.ttf", 30);
    if(!pGame->pFont || !pGame->pScoreFont){
         printf("Error: %s\n",TTF_GetError());
         close(pGame);
         return 0;
    }

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

    pGame->pPlayer[0] = createPlayer(pGame->pRenderer,160,160, 0);
    pGame->pPlayer[1] = createPlayer(pGame->pRenderer,LEVEL_WIDTH-160,160, 1);
    pGame->pPlayer[2] = createPlayer(pGame->pRenderer,LEVEL_WIDTH-160,LEVEL_HEIGHT-160, 2);
    pGame->pPlayer[3] = createPlayer(pGame->pRenderer,160,LEVEL_HEIGHT-160, 3);
    pGame->nr_of_players = MAX_PLAYERS;

    pGame->map = createGridMap();
    loadMapFromFile("../lib/resources/map.txt", pGame->map);
    pGame->gridTexture = loadGridMap(pGame->pRenderer);

    for(int i=0;i<MAX_PLAYERS;i++){
        if(!pGame->pPlayer[i]){
            printf("Error: %s\n",SDL_GetError());
            close(pGame);
            return 0;
        }
    }

    pGame->pointsUI = gameUI(pGame->pWindow, pGame->pRenderer);
    if(!pGame->pointsUI) {
        printf("Failed to create UI, Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pGame->pRenderer);
        SDL_DestroyWindow(pGame->pWindow);
        SDL_Quit();
        return 1;
    }

    pGame->currentPointsDisplayUI = pointsDisplayUI(pGame->pWindow, pGame->pRenderer, pGame->pPlayer[0]->playerPoints);
    if(!pGame->currentPointsDisplayUI) {
        printf("Failed to create currentPointsDisplayUI, Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pGame->pRenderer);
        SDL_DestroyWindow(pGame->pWindow);
        SDL_Quit();
        return 1;
    }


    pGame->flag = createFlag(pGame->pRenderer);
    if(!pGame->flag)
    {
        printf("Failed to create flag, Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pGame->pRenderer);
        SDL_DestroyWindow(pGame->pWindow);
        SDL_Quit();
        return 1;
    }

    pGame->flag->flagRect.x = pGame->map->cells[44][44].cellRect.x;
    pGame->flag->flagRect.y = pGame->map->cells[32][32].cellRect.y;
    pGame->flag->flagX = pGame->flag->flagRect.x;
    pGame->flag->flagY = pGame->flag->flagRect.y;
    pGame->flag->xStart = pGame->flag->flagRect.x;
    pGame->flag->yStart = pGame->flag->flagRect.y;


    pGame->weapon[0] = createWeapon(pGame->pRenderer, 1, pGame->map); // slow stave
    pGame->weapon[1] = createWeapon(pGame->pRenderer, 2, pGame->map); // freeze stave
    pGame->weapon[2] = createWeapon(pGame->pRenderer, 3, pGame->map); // damage stave
    pGame->weapon[3] = createWeapon(pGame->pRenderer, 4, pGame->map); // damage stave
    pGame->weapon[4] = NULL; // End of array marker (optional, but helps with iteration)

    // Ensure all weapons are created successfully
    for (int i = 0; i < 4; ++i) {
        if (!pGame->weapon[i]) {
            printf("Failed to create weapon %d, Error: %s\n", i+1, SDL_GetError());
            SDL_DestroyRenderer(pGame->pRenderer);
            SDL_DestroyWindow(pGame->pWindow);
            SDL_Quit();
            return 1;
        }
    }
    pGame->holdingFlagSound = false;
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
                    updatePlayer(pGame->pPlayer[i]);
                SDL_SetRenderDrawColor(pGame->pRenderer,255,0,0,255);
                SDL_RenderClear(pGame->pRenderer);
                SDL_SetRenderDrawColor(pGame->pRenderer,230,230,230,255);
                for(int i=0;i<MAX_PLAYERS;i++)
                {
                    playerGotFlagCheck(pGame->pPlayer[i], pGame->flag);
                    playerDeliveredFlag(pGame->pRenderer, pGame->pPlayer[i], pGame->flag, pGame->map, pGame->currentPointsDisplayUI);
                    flagAnimation(pGame->pRenderer, pGame->flag, pGame->pPlayer[0]->camera);
                    renderPlayer(pGame->pPlayer[i], pGame->pRenderer, pGame->pPlayer[i]);
                    checkCollisionWall(pGame->pPlayer[i], pGame->map);
                    checkBulletCollisionWall(pGame->pPlayer[i], pGame->map);
                    renderWeapon(pGame->pRenderer, pGame->weapon[i], pGame->pPlayer[i]->camera);
                    drawBullet(pGame->pPlayer[i]->pBullet, pGame->pRenderer, pGame->pPlayer[i]->camera);

                    if(hitPlayer(pGame->pPlayer[0], pGame->pPlayer[i], pGame->weapon, pGame->pPlayer[0]->camera))
                        printf("player %d shot player %d\n", pGame->pPlayer[0]->id, pGame->pPlayer[i]->id);
                    if(hitPlayer(pGame->pPlayer[1], pGame->pPlayer[i], pGame->weapon, pGame->pPlayer[0]->camera))
                        printf("player %d shot player %d\n", pGame->pPlayer[1]->id, pGame->pPlayer[i]->id);
                    if(hitPlayer(pGame->pPlayer[2], pGame->pPlayer[i], pGame->weapon, pGame->pPlayer[0]->camera))
                        printf("player %d shot player %d\n", pGame->pPlayer[2]->id, pGame->pPlayer[i]->id);
                    if(hitPlayer(pGame->pPlayer[3], pGame->pPlayer[i], pGame->weapon, pGame->pPlayer[0]->camera))
                        printf("player %d shot player %d\n", pGame->pPlayer[3]->id, pGame->pPlayer[i]->id);

                    applyWeaponToPlayer(pGame->pPlayer[i], pGame->weapon[0], pGame->pPlayer[0]->camera);
                    applyWeaponToPlayer(pGame->pPlayer[i], pGame->weapon[1], pGame->pPlayer[0]->camera);
                    applyWeaponToPlayer(pGame->pPlayer[i], pGame->weapon[2], pGame->pPlayer[0]->camera);
                    applyWeaponToPlayer(pGame->pPlayer[i], pGame->weapon[3], pGame->pPlayer[0]->camera);
                }

                
                SDL_RenderCopy(pGame->pRenderer, pGame->pointsUI->pointUITexture, NULL, &pGame->pointsUI->pointUIRect);
                SDL_RenderCopy(pGame->pRenderer, pGame->currentPointsDisplayUI->pointUITexture, NULL, &pGame->currentPointsDisplayUI->pointUIRect);
                SDL_RenderPresent(pGame->pRenderer);
                
                break;
            case GAME_OVER:
                sendGameData(pGame);
                if(pGame->nrOfClients==MAX_PLAYERS) pGame->nrOfClients = 0;
            case START:
                SDL_RenderPresent(pGame->pRenderer);
                if(SDL_PollEvent(&event) && event.type==SDL_QUIT) close_requested = 1;
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
    }
}

void setUpGame(Game *pGame){
    pGame->nr_of_players=MAX_PLAYERS;
    pGame->state = ONGOING;
}

void sendGameData(Game *pGame){
    pGame->sData.gState = pGame->state;
    for(int i=0;i<MAX_PLAYERS;i++){
        getPlayerSendData(pGame->pPlayer[i], &(pGame->sData.players[i]));
    }
    
    for(int i=0;i<MAX_PLAYERS;i++){
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

void executeCommand(Game *pGame, ClientData cData){
    handlePlayerInput(pGame->pPlayer[cData.playerNumber],cData,LEVEL_WIDTH,LEVEL_HEIGHT);
}

void close(Game *pGame){

    
    for(int i=0;i<MAX_PLAYERS;i++) if(pGame->pPlayer[i]) destroyPlayer(pGame->pPlayer[i]);


    for (int i = 0; i < 5; ++i) {
        if (pGame->weapon[i] != NULL) {
            if(pGame->weapon[i])destroyWeapon(pGame->weapon[i]);
        }
    }

    if(pGame->flag)destroyFlag(pGame->flag);
    if(pGame->map)destroyGridMap(pGame->map);
    if(pGame->pointsUI)(pGame->pointsUI);
    if(pGame->currentPointsDisplayUI)(pGame->currentPointsDisplayUI);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);

    if(pGame->pOverText) destroyText(pGame->pOverText);
    if(pGame->pStartText) destroyText(pGame->pStartText);   
    if(pGame->pFont) TTF_CloseFont(pGame->pFont);
    if(pGame->pScoreFont) TTF_CloseFont(pGame->pScoreFont);

    if(pGame->pPacket) SDLNet_FreePacket(pGame->pPacket);
	if(pGame->pSocket) SDLNet_UDP_Close(pGame->pSocket);

    SDLNet_Quit();
    TTF_Quit();    
    SDL_Quit();
}