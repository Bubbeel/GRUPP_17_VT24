#include "weapon.h"


Weapon* createWeapon(SDL_Renderer* renderer, int weaponId, GridMap* map) {

    Weapon* weapon = malloc(sizeof(Weapon));
    if (!weapon) {
        printf("failed to init weapon");
        return NULL; // Memory allocation failed
    }
    printf("Hello1\n");

    const char* texturePath = NULL;
    switch (weaponId) {
        case 1: 
            texturePath = "../lib/resources/tools/slow_stave.png";
            weapon->bulletType = 1;
            break;
        case 2: 
            texturePath = "../lib/resources/tools/freeze_stave.png";
            weapon->bulletType = 2; 
            break;
        case 3: 
            texturePath = "../lib/resources/tools/fire_stave.png";
            weapon->bulletType = 3; 
            break;
        case 4: 
            texturePath = "../lib/resources/tools/shield_weapon.png";
            weapon->bulletType = 4; 
            break;
        default:
            free(weapon);
            return NULL;
    }

    weapon->weapSurface = IMG_Load(texturePath);
    if (!weapon->weapSurface) {
        free(weapon);
        return NULL;
    }
    printf("Hello2\n");

    weapon->texture = SDL_CreateTextureFromSurface(renderer, weapon->weapSurface);
    SDL_FreeSurface(weapon->weapSurface);
    if (!weapon->texture) {
        printf("failed to init weapon texture");
        free(weapon);
        return NULL;
    }
    printf("Hello3\n");

    SDL_QueryTexture(weapon->texture, NULL, NULL, &weapon->rect.w, &weapon->rect.h);

    printf("Hello4\n");
    // Check if the calculated tile is a weapon tile
    if(searchWeaponTile(&weapon->rect, map)){
        weapon->posX = weapon->rect.x;
        weapon->posY = weapon->rect.y;
    }else{
        printf("failed to find weapon tile");
        free(weapon);
        return NULL;
    }
    printf("Hello5\n");

    printf("Hello6\n");
    switch (weaponId) {
        case 1: 
            weapon->rect.w = WEAPON_WIDTH;
            weapon->rect.h = 18;
            break;
        case 2: 
            weapon->rect.w = WEAPON_WIDTH;
            weapon->rect.h = 18;
            break;
        case 3: 
            weapon->rect.w = WEAPON_WIDTH;
            weapon->rect.h = 18;
            break;
        case 4: 
            weapon->rect.w = WEAPON_WIDTH;
            weapon->rect.h = WEAPON_HEIGHT;
            break;
        default:
            free(weapon);
            return NULL;
    }

    printf("Hello7\n");

    weapon->weaponId = weaponId;
    weapon->available = true;
    weapon->duration = 0;
    printf("weapon coordinates for PosX: %d\n", weapon->posX);
    printf("weapon coordinates for Posy: %d\n", weapon->posY);


    printf("Hello8\n");
    return weapon;
}

void destroyWeapon(Weapon* weapon) {
    SDL_DestroyTexture(weapon->texture);
    free(weapon);
}

void destroyWeapons(Weapon* weapons[]) {
    for (int i = 0; i < 4; i++) {
        if (weapons[i]) {
            destroyWeapon(weapons[i]);
            weapons[i] = NULL;
        }
    }
}

void renderWeapon(SDL_Renderer *renderer, Weapon* weapon, SDL_Rect camera) {
    if (weapon->available) {

        SDL_Rect destRect = {
            weapon->posX - camera.x,
            weapon->posY - camera.y,
            weapon->rect.w,
            weapon->rect.h,
        };
        SDL_RenderCopy(renderer, weapon->texture, NULL, &destRect);
    }
}

