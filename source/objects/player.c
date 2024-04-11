#include "player.h"

// Define the structure
struct Player_type {
    int positionX;
    int positionY;
};

// Function to get player's X position
int getplayerPositionX(Player p) {
    return p->positionX;
}

// Function to get player's Y position
int getplayerPositionY(Player p) {
    return p->positionY;
}

// Function to create a new player
Player createPlayer(int x, int y) {
    Player p = malloc(sizeof(struct Player_type));
    if (p != NULL) {
        p->positionX = x;
        p->positionY = y;
    }
    return p;
}
