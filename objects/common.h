#ifndef COMMON_H
#define COMMON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "../objects/player.h"

typedef struct {
    int x, y;
} PlayerPackage;

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    STOP_MOVE,
} ClientCommand; 

typedef struct {
    ClientCommand command;
    int playerNumber;
} PlayerMovementData;

#endif
