#ifndef COMMON_H
#define COMMON_H
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct{
    int x, y;
    char direction;
    int clientId;
}PlayerPackage;

#endif