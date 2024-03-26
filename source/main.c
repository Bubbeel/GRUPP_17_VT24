#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define SPEED 100
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

int main(int argv, char** args){
    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    SDL_Window* pWindow = SDL_CreateWindow("Enkelt exempel 1",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,0);
    if(!pWindow){
        printf("Error: %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!pRenderer){
        printf("Error: %s\n",SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }

    SDL_Surface *pSurface = IMG_Load("resources/ship.png");
    if(!pSurface){
        printf("Error: %s\n",SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if(!pTexture){
        printf("Error: %s\n",SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }

    SDL_Rect shipRect;
    SDL_QueryTexture(pTexture,NULL,NULL,&shipRect.w,&shipRect.h);
    shipRect.w/=4;
    shipRect.h/=4;
    float shipX = (WINDOW_WIDTH - shipRect.w)/2;//left side
    float shipY = (WINDOW_HEIGHT - shipRect.h)/2;//upper side
    float shipVelocityX = 0;//unit: pixels/s
    float shipVelocityY = 0;

    //text test
    TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24);
    SDL_Color White = {255, 255, 255};
    // as TTF_RenderText_Solid could only be used on
    // SDL_Surface then you have to create the surface first
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "put your text here", White);
    // now you can convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(pRenderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 0;  //controls the rect's x coordinate 
    Message_rect.y = 0; // controls the rect's y coordinte
    Message_rect.w = 100; // controls the width of the rect
    Message_rect.h = 100; // controls the height of the rect

    bool closeWindow = false;
    bool up,down,left,right;
    up = down = left = right = false;

    while(!closeWindow){

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    closeWindow = true;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            up=true;
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left=true;
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            down=true;
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right=true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            up=false;
                        break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left=false;
                        break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            down=false;
                        break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right=false;
                        break;
                    }
                    break;
            }
        }

        shipVelocityX = shipVelocityY = 0;
        if(up && !down) shipVelocityY = -SPEED;
        if(down && !up) shipVelocityY = SPEED;
        if(left && !right) shipVelocityX = -SPEED;
        if(right && !left) shipVelocityX = SPEED;
        shipX += shipVelocityX/60;//60 frames/s
        shipY += shipVelocityY/60;
        if(shipX<0) shipX=0;
        if(shipY<0) shipY=0;
        if(shipX>WINDOW_WIDTH-shipRect.w) shipX = WINDOW_WIDTH-shipRect.w;
        if(shipY>WINDOW_HEIGHT-shipRect.h) shipY = WINDOW_HEIGHT-shipRect.h;
        shipRect.x = shipX;
        shipRect.y = shipY;

        SDL_RenderClear(pRenderer);
        SDL_RenderCopy(pRenderer,pTexture,NULL,&shipRect);
        SDL_RenderPresent(pRenderer);
        SDL_Delay(1000/60);//60 frames/s
    }

    SDL_DestroyTexture(pTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    SDL_Quit();
    return 0;
}