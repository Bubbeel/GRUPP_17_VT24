#include "flag.h"
#include <math.h>

void moveFlag(SDL_Rect *flagRect, float player1X, float player1Y, float player2X, float player2Y, float closeDistanceThreshold, float flagSpeed) {
    // Calculate distance between player 1 and flag
    float distToPlayer1 = sqrt(pow(player1X - flagRect->x, 2) + pow(player1Y - flagRect->y, 2));

    // Calculate distance between player 2 and flag
    float distToPlayer2 = sqrt(pow(player2X - flagRect->x, 2) + pow(player2Y - flagRect->y, 2));

    if (distToPlayer1 < distToPlayer2 && distToPlayer1 < closeDistanceThreshold) {
        float dx = player1X - flagRect->x;
        float dy = player1Y - flagRect->y;
        float length = sqrt(dx * dx + dy * dy);
        if (length != 0) {
            dx /= length;
            dy /= length;
        }
        flagRect->x += dx * flagSpeed;
        flagRect->y += dy * flagSpeed;
    }
    else if (distToPlayer2 < distToPlayer1 && distToPlayer2 < closeDistanceThreshold) {
        float dx = player2X - flagRect->x;
        float dy = player2Y - flagRect->y;
        float length = sqrt(dx * dx + dy * dy);
        if (length != 0) {
            dx /= length;
            dy /= length;
        }
        flagRect->x += dx * flagSpeed;
        flagRect->y += dy * flagSpeed;
    }
}