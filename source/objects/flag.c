#include "flag.h"
#include <math.h>


void moveFlag(SDL_Rect *flagRect, float player1X, float player1Y, float player2X, float player2Y, float closeDistanceThreshold, float flagSpeed) {

    float distToPlayer1 = sqrt(pow(player1X - flagRect->x, 2) + pow(player1Y - flagRect->y, 2));

    float distToPlayer2 = sqrt(pow(player2X - flagRect->x, 2) + pow(player2Y - flagRect->y, 2));

    if (distToPlayer1 < closeDistanceThreshold && distToPlayer1 < distToPlayer2) {
        flagRect->x = player1X;
        flagRect->y = player1Y;
    }
 
    if (distToPlayer2 < closeDistanceThreshold && distToPlayer2 < distToPlayer1) {
        flagRect->x = player2X;
        flagRect->y = player2Y;
    }
}