#ifndef GAMEDATA_H
#define GAME_H

//UNDER CONSTRUCTION TO BE IMPLEMENTED IN THE FUTURE

//#define WINDOW_WIDTH 1408
//#define WINDOW_HEIGHT 800

struct playerData{
    float x, y, vx, vy;
    int angle, alive;
    //BulletData bData;
};
typedef struct playerData PlayerData; 

#endif // GAMEDATA_H