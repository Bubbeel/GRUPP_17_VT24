#ifndef player_h
#define player_h

#include <stdio.h>
#include <stdlib.h>

typedef struct Player_type *Player;

int getplayerPositionX(Player p);
int getplayerPositionY(Player p);
Player createPlayer(int x, int y);

#endif