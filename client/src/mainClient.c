#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>
#include "music.h"
#include "player.h"
#include "gridmap.h"
#include "playerData.h"
#include "weapon.h"
#include "collisionDetection.h"
#include "flag.h"
#include "menu.h"
#include "text.h"

#define PLAYER_FRAME_RATE 60

struct game{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    SDL_Texture* gridTexture;
    Player* pPlayer[MAX_PLAYERS];
    int nr_of_players, playerNr;
    GridMap* map;
    Weapon* weapon[5];
    Flag* flag;
    GameUI* pointsUI;
    GameUI* currentPointsDisplayUI;
    int* joining;
    TTF_Font *pFont, *pScoreFont;
    Text *pOverText, *pStartText, *pWaitingText, *pYouLoseText, *pYouWonText;
    bool holdingFlagSound;
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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL2 Mixer: %s", Mix_GetError());
        SDL_Quit();
        return 0;
    }

    if (!initAudio()) {
        printf("Failed to initialize audio, initAudio Error: %s\n", SDL_GetError());
        closeAudio();
        SDL_Quit();
        return 1;
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


    pGame->pFont = TTF_OpenFont("../lib/resources/arial.ttf", 100);
    pGame->pScoreFont = TTF_OpenFont("../lib/resources/arial.ttf", 30);

    if(!pGame->pFont || !pGame->pScoreFont){
         printf("Error: %s\n",TTF_GetError());
         close(pGame);
         return 0;
    }


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



    pGame->map = createGridMap();
    loadMapFromFile("../lib/resources/map.txt", pGame->map);
    pGame->gridTexture = loadGridMap(pGame->pRenderer);


    pGame->pPlayer[0] = createPlayer(pGame->pRenderer,160,160, 0);
    pGame->pPlayer[1] = createPlayer(pGame->pRenderer,LEVEL_WIDTH-160,160, 1);
    pGame->pPlayer[2] = createPlayer(pGame->pRenderer,LEVEL_WIDTH-160,LEVEL_HEIGHT-160, 2);
    pGame->pPlayer[3] = createPlayer(pGame->pRenderer,160,LEVEL_HEIGHT-160, 3);
    printf("Player created\n");
    pGame->nr_of_players = MAX_PLAYERS;
    for(int i=0;i<MAX_PLAYERS;i++){
        if(!pGame->pPlayer[i]){
            printf("Error: %s\n",SDL_GetError());
            close(pGame);
            return 0;
        }
    }

    pGame->pOverText = createText(pGame->pRenderer,238,168,65,pGame->pFont,"Game over",WINDOW_WIDTH/2,WINDOW_HEIGHT/2-300);
    pGame->pYouWonText = createText(pGame->pRenderer,238,168,65,pGame->pFont,"You Won!",WINDOW_WIDTH/2,WINDOW_HEIGHT/2);
    pGame->pYouLoseText = createText(pGame->pRenderer,238,168,65,pGame->pFont,"You Lost!",WINDOW_WIDTH/2,WINDOW_HEIGHT/2);
    pGame->pStartText = createText(pGame->pRenderer,238,168,65,pGame->pScoreFont,"Waiting for clients",WINDOW_WIDTH/2,WINDOW_HEIGHT/2+100);
    pGame->pWaitingText = createText(pGame->pRenderer,238,168,65,pGame->pScoreFont,"Waiting for clients",WINDOW_WIDTH/2,WINDOW_HEIGHT/2);


    if(!pGame->pOverText || !pGame->pStartText || !pGame->pWaitingText || !pGame->pYouWonText || !pGame->pYouLoseText){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;
    }

    pGame->pointsUI = gameUI(pGame->pWindow, pGame->pRenderer);
    if(!pGame->pointsUI) {
        printf("Failed to create UI, Error: %s\n", SDL_GetError());
        close(pGame);
        return 1;
    }

    pGame->currentPointsDisplayUI = pointsDisplayUI(pGame->pWindow, pGame->pRenderer, pGame->pPlayer[0]->playerPoints);
    if(!pGame->currentPointsDisplayUI) {
        printf("Failed to create currentPointsDisplayUI, Error: %s\n", SDL_GetError());
        close(pGame);
        return 1;
    }


    pGame->flag = createFlag(pGame->pRenderer);
    if(!pGame->flag)
    {
        printf("Failed to create flag, Error: %s\n", SDL_GetError());
        close(pGame);
        return 1;
    }
    //44 44 32 32
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
            close(pGame);
            return 1;
        }
    }

    pGame->joining = malloc(sizeof(int));
    if (pGame->joining == NULL) {
        close(pGame);
        return 1;
    }

    pGame->holdingFlagSound = false;
    pGame->state = START;

    return 1;
}

//game
void run(Game *pGame){
    int close_requested = 0;
    SDL_Event event;
    ClientData cData;
    
    *pGame->joining = 0;
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
                SDL_SetRenderDrawColor(pGame->pRenderer,0,0,0,255);
                SDL_RenderClear(pGame->pRenderer); 
                SDL_SetRenderDrawColor(pGame->pRenderer,230,230,230,255);
                renderGridMapCentered(pGame->pRenderer, pGame->map, pGame->pPlayer[pGame->playerNr], pGame->gridTexture, WINDOW_WIDTH, WINDOW_HEIGHT, LEVEL_WIDTH, LEVEL_HEIGHT);
                for(int i=0;i<MAX_PLAYERS;i++)
                {   
                    flagAnimation(pGame->pRenderer, pGame->flag, pGame->pPlayer[pGame->playerNr]->camera);
                    playerGotFlagCheck(pGame->pPlayer[i], pGame->flag);
                    playerDeliveredFlag(pGame->pRenderer, pGame->pPlayer[i], pGame->flag, pGame->map, pGame->currentPointsDisplayUI); 
                    renderPlayer(pGame->pPlayer[i], pGame->pRenderer, pGame->pPlayer[pGame->playerNr]);
                    checkCollisionWall(pGame->pPlayer[i], pGame->map);
                    checkBulletCollisionWall(pGame->pPlayer[i], pGame->map);

                    if(pGame->pPlayer[i]->id == 0){
                        renderWeapon(pGame->pRenderer, pGame->weapon[0], pGame->pPlayer[pGame->playerNr]->camera);
                        renderWeapon(pGame->pRenderer, pGame->weapon[1], pGame->pPlayer[pGame->playerNr]->camera);
                        renderWeapon(pGame->pRenderer, pGame->weapon[2], pGame->pPlayer[pGame->playerNr]->camera);
                        renderWeapon(pGame->pRenderer, pGame->weapon[3], pGame->pPlayer[pGame->playerNr]->camera);
                    }
                    updatePlayer(pGame->pPlayer[i]);
                    drawBullet(pGame->pPlayer[i]->pBullet, pGame->pRenderer, pGame->pPlayer[pGame->playerNr]->camera);

                    if(hitPlayer(pGame->pPlayer[0], pGame->pPlayer[i], pGame->weapon, pGame->pPlayer[0]->camera))
                        printf("player %d shot player %d\n", pGame->pPlayer[0]->id, pGame->pPlayer[i]->id);
                    if(hitPlayer(pGame->pPlayer[1], pGame->pPlayer[i], pGame->weapon, pGame->pPlayer[0]->camera))
                        printf("player %d shot player %d\n", pGame->pPlayer[1]->id, pGame->pPlayer[i]->id);
                    if(hitPlayer(pGame->pPlayer[2], pGame->pPlayer[i], pGame->weapon, pGame->pPlayer[0]->camera))
                        printf("player %d shot player %d\n", pGame->pPlayer[2]->id, pGame->pPlayer[i]->id);
                    if(hitPlayer(pGame->pPlayer[3], pGame->pPlayer[i], pGame->weapon, pGame->pPlayer[0]->camera))
                        printf("player %d shot player %d\n", pGame->pPlayer[3]->id, pGame->pPlayer[i]->id);

                    applyWeaponToPlayer(pGame->pPlayer[i], pGame->weapon[0], pGame->pPlayer[pGame->playerNr]->camera);
                    applyWeaponToPlayer(pGame->pPlayer[i], pGame->weapon[1], pGame->pPlayer[pGame->playerNr]->camera);
                    applyWeaponToPlayer(pGame->pPlayer[i], pGame->weapon[2], pGame->pPlayer[pGame->playerNr]->camera);
                    applyWeaponToPlayer(pGame->pPlayer[i], pGame->weapon[3], pGame->pPlayer[pGame->playerNr]->camera);
                    
                    if(pGame->pPlayer[i]->playerPoints == 3)
                        drawText(pGame->pOverText);
                    if(pGame->pPlayer[pGame->playerNr]->playerPoints == 3)
                        drawText(pGame->pYouWonText);
                    if(pGame->pPlayer[pGame->playerNr]-> playerPoints != 3 && pGame->pPlayer[i]->playerPoints == 3)
                        drawText(pGame->pYouLoseText);
                }
                SDL_RenderCopy(pGame->pRenderer, pGame->pointsUI->pointUITexture, NULL, &pGame->pointsUI->pointUIRect);
                SDL_RenderCopy(pGame->pRenderer, pGame->currentPointsDisplayUI->pointUITexture, NULL, &pGame->currentPointsDisplayUI->pointUIRect);
                SDL_RenderPresent(pGame->pRenderer);
                break;
            case GAME_OVER:
                drawText(pGame->pOverText);
            case START:
                if(!*pGame->joining){
                    displayMenu(pGame->pWindow, pGame->pRenderer, pGame->joining);
                }else{
                    SDL_SetRenderDrawColor(pGame->pRenderer,0,0,0,255);
                    SDL_RenderClear(pGame->pRenderer);
                    SDL_SetRenderDrawColor(pGame->pRenderer,230,230,230,255);
                    drawText(pGame->pWaitingText);
                }
                SDL_RenderPresent(pGame->pRenderer);
                if(SDL_PollEvent(&event)){
                    if(event.type==SDL_QUIT) close_requested = 1;
                    else if(*pGame->joining){ 
                        *pGame->joining = 1;
                        cData.playerNumber=-1;
                        memcpy(pGame->pPacket->data, &cData, sizeof(ClientData));
		                pGame->pPacket->len = sizeof(ClientData);
                    }
                }
                if(*pGame->joining) SDLNet_UDP_Send(pGame->pSocket, -1,pGame->pPacket);
                if(SDLNet_UDP_Recv(pGame->pSocket, pGame->pPacket)){
                    printf("Server sent packet\n");
                    updateWithServerData(pGame);
                    if(pGame->state == ONGOING) *pGame->joining = 0;
                }                
                break;
        }
        SDL_Delay(1000 / PLAYER_FRAME_RATE);
    }
}





void updateWithServerData(Game *pGame){
    ServerData sData;
    memcpy(&sData, pGame->pPacket->data, sizeof(ServerData));
    pGame->playerNr = sData.playerNr;
    pGame->pPlayer[pGame->playerNr]->playerNumber = pGame->playerNr;
    pGame->state = sData.gState;

    for(int i=0;i<MAX_PLAYERS;i++){
        updatePlayerWithRecievedData(pGame->pPlayer[i],&(sData.players[i]));
    }
}





void handleInput(Game *pGame, SDL_Event *pEvent) {
    static ClientData cData;
    cData.playerNumber = pGame->playerNr;

    if (pEvent->type == SDL_KEYDOWN || pEvent->type == SDL_KEYUP) {
        bool isPressed = (pEvent->type == SDL_KEYDOWN);

        switch (pEvent->key.keysym.scancode) {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                cData.command.UP = isPressed;
                if(cData.command.UP == isPressed){
                    playWalkingSound();
                }

                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                cData.command.LEFT = isPressed;
                if(cData.command.LEFT == isPressed){
                    playWalkingSound();
                }
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                cData.command.RIGHT = isPressed;
                if(cData.command.RIGHT == isPressed){
                    playWalkingSound();
                }
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                cData.command.DOWN = isPressed;
                if(cData.command.DOWN == isPressed){
                    playWalkingSound();
                }
                break;
            case SDL_SCANCODE_SPACE:
                cData.command.FIRE = isPressed;
                if(cData.command.FIRE == isPressed){
                    playShootSound();
                }
                break;
        }

        cData.x = pGame->pPlayer[pGame->playerNr]->playerX;
        cData.y = pGame->pPlayer[pGame->playerNr]->playerY;
        memcpy(pGame->pPacket->data, &cData, sizeof(ClientData));
        pGame->pPacket->len = sizeof(ClientData);
        SDLNet_UDP_Send(pGame->pSocket, -1, pGame->pPacket);
    }
}






void close(Game *pGame){
    if(pGame->flag)destroyFlag(pGame->flag);
    if(pGame->map)destroyGridMap(pGame->map);

    for(int i=0;i<MAX_PLAYERS;i++)
    {
        if(pGame->pPlayer[i])(pGame->pPlayer[i]);
    }

    for (int i = 0; i < 5; ++i) {
        if (pGame->weapon[i] != NULL) {
            if(pGame->weapon[i])(pGame->weapon[i]);
        }
    }

    if(pGame->pointsUI)(pGame->pointsUI);
    if(pGame->currentPointsDisplayUI)destroyCurrentPointsUI(pGame->currentPointsDisplayUI);


    if(pGame->pYouWonText) destroyText(pGame->pYouWonText);
    if(pGame->pYouLoseText) destroyText(pGame->pYouLoseText);
    if(pGame->pWaitingText) destroyText(pGame->pWaitingText);
    if(pGame->pOverText) destroyText(pGame->pOverText);
    if(pGame->pStartText) destroyText(pGame->pStartText);   
    if(pGame->pFont) TTF_CloseFont(pGame->pFont);
    if(pGame->pScoreFont) TTF_CloseFont(pGame->pScoreFont);

    if(pGame->joining)(pGame->joining);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);

    closeAudio();
    SDLNet_Quit();
    TTF_Quit();    
    SDL_Quit();
}