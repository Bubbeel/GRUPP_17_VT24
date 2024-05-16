#ifndef playerData_h
#define playerData_h

#define WINDOW_WIDTH 1408
#define WINDOW_HEIGHT 800
#define MAX_PLAYERS 2

enum gameState{START, ONGOING, GAME_OVER};
typedef enum gameState GameState;

enum clientCommand{READY, ACC, LEFT, RIGHT, FIRE};
typedef enum clientCommand ClientCommand;

struct clientData{
    ClientCommand command;
    int playerNumber;
};
typedef struct clientData ClientData;

struct bulletData{
    float x, y, vx, vy;
    int time;
};
typedef struct bulletData BulletData;

struct playerData{
    float x, y, vx, vy;
    int alive;
    BulletData bData;
};
typedef struct playerData PlayerData;   

struct serverData{
    PlayerData players[MAX_PLAYERS];
    int playerNr;
    GameState gState;
};
typedef struct serverData ServerData;

#endif 