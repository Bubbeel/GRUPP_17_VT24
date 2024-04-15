//
//  map.h
//
//  Created by Jonas Willén on 2021-03-29.
//

#ifndef map_h
#define map_h
#include <stdio.h>

int getTileRows();
int getTileColumns();
int getTileWidth();
int getTileHeight();
int getTileGrid(int x, int y);
int collidesWithImpassableTile(int x, int y);

#endif /* map_h */