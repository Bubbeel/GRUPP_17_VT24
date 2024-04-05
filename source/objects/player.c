#include "player.h"

#define PUBLIC
#define PRIVATE static

PRIVATE int width = 16;
PRIVATE int height = 16;

struct Player_type
{
    int PLayer_POSITIOn_Y;
    int PLayer_POSITIOn_X;
    int PLayer_frame;
    int speed;
};
PUBLIC Player createPlayer(int x, int y){   
Player p = malloc(sizeof(struct Player_type));

    p->PLayer_POSITIOn_Y = y;
    p->PLayer_POSITIOn_X = x;
    p->PLayer_frame = 0;
    p->speed = 1;
    return p;

}


PUBLIC int getPlayerHeight(){
    return height;
}


PUBLIC int getPlayerWidth(){
    return width;   
}