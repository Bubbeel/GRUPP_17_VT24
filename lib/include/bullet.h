#ifndef BULLET_h
#define BULLET_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "playerData.h"

#define BULLETLIFETIME 150

struct bullet{
    float x, y, vx, vy;
    int time, window_width, window_height;
    int type;
    int frames;
    SDL_Rect rect;
};
typedef struct bullet Bullet;

Bullet *createBullet(SDL_Renderer *pRenderer, int window_width, int window_height);
void updateBullet(Bullet *pBullet);
void startBullet(Bullet *pBullet, float x, float y, float vx, float vy, SDL_Rect camera);
void killBullet(Bullet *pBullet);
void drawBullet(Bullet *pBullet, SDL_Renderer *pRenderer, SDL_Rect camera); // with camera
float xBullet(Bullet *pBullet);
float yBullet(Bullet *pBullet);
void destroyBullet(Bullet *pBullet);
int aliveBullet(Bullet *pBullet);
void getBulletSendData(Bullet* pBullet, BulletData* BulletData);
void updateBulletWithRecievedData(Bullet* pBullet, BulletData* BulletData);

#endif