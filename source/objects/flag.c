#include "flag.h"
#include <math.h>


void moveFlag(SDL_Rect *flagRect, int player1X, int player1Y, int player2X, int player2Y, int closeDistanceThreshold, int flagSpeed) {

    int distToPlayer1 = sqrt(pow(player1X - flagRect->x, 2) + pow(player1Y - flagRect->y, 2));

    int distToPlayer2 = sqrt(pow(player2X - flagRect->x, 2) + pow(player2Y - flagRect->y, 2));

    if (distToPlayer1 < closeDistanceThreshold && distToPlayer1 < distToPlayer2) {
        flagRect->x = player1X;
        flagRect->y = player1Y;
    }
 
    if (distToPlayer2 < closeDistanceThreshold && distToPlayer2 < distToPlayer1) {
        flagRect->x = player2X;
        flagRect->y = player2Y;
    }
}