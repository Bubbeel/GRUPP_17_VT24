#ifndef FLAG_H
#define FLAG_H

#include <SDL2/SDL.h>

void moveFlag(SDL_Rect *flagRect, int player1X, int player1Y, int player2X, int player2Y, int closeDistanceThreshold, int flagSpeed);

#endif /* FLAG_H */
