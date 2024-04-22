#include "flag.h"

Flag* createFlag(int x, int y) {
    Flag* flag = (Flag*)malloc(sizeof(Flag));
    if (flag != NULL) {
        flag->x = x;
        flag->y = y;
    }
    return flag;
}

void destroyFlag(Flag* flag) {
    if (flag != NULL) {
        free(flag);
    }
}

void updateFlagPosition(Flag* flag, int x, int y) {
    if (flag != NULL) {
        flag->x = x;
        flag->y = y;
    }
}
