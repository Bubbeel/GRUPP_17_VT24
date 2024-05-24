#include "bullet.h"


Bullet *createBullet(SDL_Renderer* pRenderer, int window_width, int window_height){
    Bullet *pBullet = malloc(sizeof(Bullet));
    if (!pBullet) {
        return NULL;
    }
/*
    pBullet->bulletTextures[0] = IMG_LoadTexture(pRenderer, "resources/tools/normal_shot.png");
    if (pBullet->bulletTextures[0] == NULL) {
        return NULL;
    }

    pBullet->bulletTextures[1] = IMG_LoadTexture(pRenderer, "resources/tools/slow_shot.png");
    if (pBullet->bulletTextures[1] == NULL) {
        SDL_DestroyTexture(pBullet->bulletTextures[0]);
        return NULL;
    }

    pBullet->bulletTextures[2] = IMG_LoadTexture(pRenderer, "resources/tools/poison_shot.png");
    if (pBullet->bulletTextures[2] == NULL) {
        SDL_DestroyTexture(pBullet->bulletTextures[0]);
        SDL_DestroyTexture(pBullet->bulletTextures[1]);
        return NULL;
    }

    pBullet->bulletTextures[3] = NULL;
*/

    pBullet->window_width = window_width;
    pBullet->window_height = window_height;
    pBullet->time = 0;
    pBullet->type = 0;
    pBullet->frames = 0;

    //pBullet->textureId = 0;

    printf("Created bullet succesfully\n");

    return pBullet;
}

void updateBullet(Bullet *pBullet){ // in loop
    if(pBullet->time==0) return;

    pBullet->x+=pBullet->vx;
    pBullet->y+=pBullet->vy;
    pBullet->rect.x = pBullet->x;
    pBullet->rect.y = pBullet->y;

    if(pBullet->x<0) pBullet->x+=pBullet->window_width;
    else if (pBullet->x>pBullet->window_width) pBullet->x-=pBullet->window_width;

    if(pBullet->y<0) pBullet->y+=pBullet->window_height;
    else if(pBullet->y>pBullet->window_height) pBullet->y-=pBullet->window_height;
    //printf("current bulletLife: %d \n", pBullet->time);
    (pBullet->time)--;
    //printf("current bulletLife: %d \n", pBullet->time);
    return;
}

void startBullet(Bullet *pBullet, float x, float y, float vx, float vy, SDL_Rect camera){
    pBullet->x = x;
    pBullet->y = y;
    pBullet->vx = vx;
    pBullet->vy = vy;

    pBullet->rect.x = pBullet->x - camera.x;
    pBullet->rect.y = pBullet->y - camera.y;
    pBullet->rect.w = 15;
    pBullet->rect.h = 15;
    
    pBullet->time = 150;

}

void killBullet(Bullet *pBullet){
    pBullet->time=0;
}

void drawBullet(Bullet *pBullet, SDL_Renderer *pRenderer, SDL_Rect camera){
    if(pBullet->time==0) return;
/*
    SDL_Texture *bulletTexture = pBullet->bulletTextures[pBullet->textureId];
    if (bulletTexture == NULL) return;
    //for(int i=0; pBullet->bulletTextures[i] != NULL; i++){
        //if (pBullet->bulletTextures[i] == NULL) return;
    //}

    int texWidth, texHeight;
    SDL_QueryTexture(bulletTexture, NULL, NULL, &texWidth, &texHeight);
*/
    //SDL_RenderDrawPoint(pRenderer,pBullet->x,pBullet->y);
    //SDL_RenderDrawPoint(pRenderer,pBullet->x+1,pBullet->y);
    //SDL_RenderDrawPoint(pRenderer,pBullet->x,pBullet->y+1);
    //SDL_RenderDrawPoint(pRenderer,pBullet->x+1,pBullet->y+1);
/*
    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = texWidth; // Width of the entire texture
    srcRect.h = texHeight; // Height of the entire texture
*/
    SDL_Rect bulletRect;
    bulletRect.x = pBullet->x-camera.x;
    bulletRect.y = pBullet->y-camera.y;
    bulletRect.w = 15; // Width of the bullet
    bulletRect.h = 15; // Height of the bullet

    // Set the draw color for the bullet (optional, you can set it before calling this function)
    SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255); // Red color for the bullet
    // SDL_RenderCopy(pRenderer, pBullet->bulletTextures[pBullet->textureId], &srcRect, &bulletRect);
    // Draw the filled rectangle representing the bullet
    SDL_RenderFillRect(pRenderer, &bulletRect);
}

float xBullet(Bullet *pBullet){
    return pBullet->x;
}

float yBullet(Bullet *pBullet){
    return pBullet->y;
}

void destroyBullet(Bullet *pBullet){
    free(pBullet);
}

int aliveBullet(Bullet *pBullet){
    return pBullet->time>0;
}

void getBulletSendData(Bullet* pBullet, BulletData* pBulletData){
    pBulletData->time = pBullet->time;
    pBulletData->vx = pBullet->vx;
    pBulletData->vy = pBullet->vy;
    pBulletData->x = pBullet->x;
    pBulletData->y = pBullet->y;
}

void updateBulletWithRecievedData(Bullet *pBullet,BulletData *pBulletData){
    pBullet->time = pBulletData->time;
    pBullet->vx = pBulletData->vx;
    pBullet->vy = pBulletData->vy;
    pBullet->x = pBulletData->x;
    pBullet->y = pBulletData->y;
}
