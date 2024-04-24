#ifndef FLAG_H
#define FLAG_H

#include <SDL2/SDL.h>

void moveFlag(SDL_Rect *flagRect, float player1X, float player1Y, float player2X, float player2Y, float closeDistanceThreshold, float flagSpeed);

#endif /* FLAG_H */
