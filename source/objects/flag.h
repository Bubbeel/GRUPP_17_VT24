// flag.h
#ifndef FLAG_H
#define FLAG_H

#include <stdio.h>

// Flag structure
typedef struct {
    int x;  // X coordinate of the flag
    int y;  // Y coordinate of the flag
} Flag;

// Function declarations
Flag* createFlag(int x, int y);
void destroyFlag(Flag* flag);
void updateFlagPosition(Flag* flag, int x, int y);

#endif // FLAG_H
