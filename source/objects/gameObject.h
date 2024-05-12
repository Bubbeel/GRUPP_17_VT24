#ifndef gameobject_h
#define gameobject_h

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct
{
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Surface surface;
    char tag;
} GameObject;

#endif //gameobject_h