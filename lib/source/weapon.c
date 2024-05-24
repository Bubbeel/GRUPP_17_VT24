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

/*
    weapon->texture = IMG_LoadTexture(renderer, texturePath);
    if (!weapon->texture) {
        printf("failed to init weapon texture");
        free(weapon);
        return NULL; // Texture loading failed
    }


*/

    // Calculate tile coordinates corresponding to the weapon's position
    //int tileX = weapon->rect.x / CELL_SIZE;
    //int tileY = weapon->rect.y / CELL_SIZE;
    printf("Hello4\n");
    // Check if the calculated tile is a weapon tile
    if(searchWeaponTile(&weapon->rect, map)){
        weapon->posX = weapon->rect.x;
        weapon->posY = weapon->rect.y;
    }else{
        printf("failed to find weapon tile\n");
        free(weapon);
        return NULL;
    }
    printf("Hello5\n");
/*
    printf("Hello4\n");
    weapon->rect.x = 0;
    weapon->rect.y = 0;


    srand(time(NULL));
    printf("Hello5\n");
    while (!searchWeaponTile(weapon->rect.x, weapon->rect.y, map) ||
            !searchWeaponTile(weapon->rect.x + 20, weapon->rect.y + 20, map) || 
            !searchWeaponTile(weapon->rect.x - 20, weapon->rect.y - 20, map)){
        weapon->rect.x = rand() % 2816; //
        printf("randNr: %d\n", weapon->rect.x);
        weapon->rect.y = rand() % 2100;
        printf("randNr: %d\n", weapon->rect.y);
        printf("Searching\n");
    } 
*/
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
/*
    switch (weaponId) {
        case 1:
            weapon->bulletType = 1; // slow down effect
            break;
        case 2:
            weapon->bulletType = 2; // frozen down effect
            break;
        case 3:
            weapon->bulletType = 3; // damage effect
            break;
        case 4:
            weapon->bulletType = 4; // shield effect
            break;
        default:
            break;
    }
*/
    printf("Hello7\n");

    weapon->weaponId = weaponId;
    weapon->available = true;
    weapon->duration = 0;
    // weapon->posX = weapon->posY = 0;
    printf("weapon coordinates for PosX: %d\n", weapon->posX);
    printf("weapon coordinates for Posy: %d\n", weapon->posY);


    printf("Hello8\n");
    return weapon;
}

void destroyWeapon(Weapon* weapon) {
    SDL_DestroyTexture(weapon->texture);
    free(weapon);
}

/*
Weapon* initWeapons(SDL_Renderer* renderer, int weaponId, GridMap* map) { // put in the create weapon instead
    const char* texturePaths[] = {
        "resources/tools/slow_stave.png",  // SlowDownStave
        "resources/tools/freeze_stave.png",  // FreezeStave
        "resources/tools/fire_stave.png",  // DamageStave
        "resources/tools/shield_weapon.png"   // ProtectionShield
    };

    if (weaponId < 1 || weaponId > 4) {
        weaponId = 1; // Default to SlowDownStave if weaponId is invalid
    }

    if (!texturePaths[weaponId - 1]) {
        printf("Invalid texture path\n");
        return NULL;
    }

    return createWeapon(renderer, texturePaths[weaponId - 1], weaponId, map);
}
*/

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


/*
 Weapon* initWeapons(SDL_Renderer *renderer, int weaponId){

    Weapon* pSlowDownStave = malloc(sizeof(Weapon));
    if (!pSlowDownStave) {
        return NULL;
    }

    pSlowDownStave->texture = IMG_LoadTexture(renderer, "resources/perk_1.png");
    if (!pSlowDownStave->texture) {
        free(pSlowDownStave);
        return NULL;
    }

    pSlowDownStave->available = true;
    pSlowDownStave->weaponId = 1;

    Weapon* pFreezeStave = malloc(sizeof(Weapon));
    if (!pFreezeStave) {
        SDL_DestroyTexture(pSlowDownStave->texture);
        free(pSlowDownStave);
        return NULL;
    }

    pFreezeStave->texture = IMG_LoadTexture(renderer, "resources/perk_1.png");
    if (!pFreezeStave->texture) {
        SDL_DestroyTexture(pSlowDownStave->texture);
        free(pFreezeStave);
        free(pSlowDownStave);
        return NULL;
    }

    pFreezeStave->available = true;
    pFreezeStave->weaponId = 2;

    Weapon* pDamageStave = malloc(sizeof(Weapon));
    if (!pDamageStave) {
        SDL_DestroyTexture(pFreezeStave->texture);
        SDL_DestroyTexture(pSlowDownStave->texture);
        free(pFreezeStave);
        free(pSlowDownStave);
        return NULL;
    }

    pDamageStave->texture = IMG_LoadTexture(renderer, "resources/perk_1.png");
    if (!pDamageStave->texture) {
        SDL_DestroyTexture(pFreezeStave->texture);
        SDL_DestroyTexture(pSlowDownStave->texture);
        free(pDamageStave);
        free(pFreezeStave);
        free(pSlowDownStave);
        return NULL;
    }

    pDamageStave->available = true;
    pDamageStave->weaponId = 3;

    Weapon* pProtectionShield = malloc(sizeof(Weapon));
    if (!pProtectionShield) {
        SDL_DestroyTexture(pDamageStave->texture);
        SDL_DestroyTexture(pFreezeStave->texture);
        SDL_DestroyTexture(pSlowDownStave->texture);
        free(pDamageStave);
        free(pFreezeStave);
        free(pSlowDownStave);
        return NULL;
    }

    pProtectionShield->texture = IMG_LoadTexture(renderer, "resources/perk_1.png");
    if (!pProtectionShield->texture) {
        SDL_DestroyTexture(pDamageStave->texture);
        SDL_DestroyTexture(pFreezeStave->texture);
        SDL_DestroyTexture(pSlowDownStave->texture);
        free(pProtectionShield);
        free(pDamageStave);
        free(pFreezeStave);
        free(pSlowDownStave);
        return NULL;
    }

    pProtectionShield->available = true;
    pProtectionShield->weaponId = 4;

*/
/*
    slowDownStave.texture = IMG_LoadTexture(renderer, "resources/perk_1.png");
    freezeStave.texture = IMG_LoadTexture(renderer, "resources/perk_2.png");
    damageStave.texture = IMG_LoadTexture(renderer, "resources/perk_3.png");
    if (perkTextureSpeed == NULL || perkTextureFreeze == NULL || perkTextureReverse == NULL) {
        printf("Failed to load perk sprite sheet: %s\n", IMG_GetError());
        exit(1);
    }
*/

    //speedBoostPerk.texture = perkTextureSpeed;
    //freezePerk.texture = perkTextureFreeze;
    //reverseKeysPerk.texture = perkTextureReverse;
/*
    srand(time(NULL));
    int randomLocationX[4];
    int randomLocationY[4];

    for (int i = 0; i < 4; i++)
    {
        do {
            randomLocationX[i] = rand() % 2280;
            randomLocationY[i] = rand() % 720;
        } while (encountersForbiddenTile(randomLocationX[i], randomLocationY[i]) || encountersForbiddenTile(randomLocationX[i] + 20, randomLocationY[i] + 20) || encountersForbiddenTile(randomLocationX[i] - 20, randomLocationY[i] - 20));
    }

    speedBoostPerk.rect.x = randomLocationX[0];
    speedBoostPerk.rect.y = randomLocationY[0];
    freezePerk.rect.x = randomLocationX[1];
    freezePerk.rect.y = randomLocationY[1];
    reverseKeysPerk.rect.x = randomLocationX[2];
    reverseKeysPerk.rect.y = randomLocationY[2];

    speedBoostPerk.rect.w = freezePerk.rect.w = reverseKeysPerk.rect.w = WEAPON_WIDTH;
    speedBoostPerk.rect.h = freezePerk.rect.h = reverseKeysPerk.rect.h = WEAPON_HEIGHT;
    speedBoostPerk.available = freezePerk.available = reverseKeysPerk.available = true;
    speedBoostPerk.duration = freezePerk.duration = reverseKeysPerk.duration = 0;    // Initialize duration to 0
    //speedBoostPerk.respawnTime = freezePerk.duration = reverseKeysPerk.duration = 0; // Initialize respawn time to 0

    if (weaponId == 1) {
        speedBoostPerk.weaponId = weaponId;
        return speedBoostPerk;
    }
    else if (weaponId == 2) {
        freezePerk.weaponId = weaponId;
        return freezePerk;
    }
    else if (weaponId == 3) {
        reverseKeysPerk.weaponId = weaponId;
        return reverseKeysPerk;
    }
    else {
        // Return a default perk if perkNr is invalid
        speedBoostPerk.weaponId = weaponId;
        return speedBoostPerk;
    }
 }
*/
/*
 void destroyWeapons(Weapon* weaponSlow1, Weapon* weaponFreeze2, Weapon* weaponDamage3, Weapon* weaponShield4) {
        SDL_DestroyTexture(weaponSlow1->texture);
        SDL_DestroyTexture(weaponFreeze2->texture);
        SDL_DestroyTexture(weaponDamage3->texture);
        SDL_DestroyTexture(weaponShield4->texture);
        free(weaponSlow1);
        free(weaponFreeze2);
        free(weaponDamage3);
        free(weaponShield4);
}
*/

/*
 static void getStartValues(Asteroid *pAsteroid){
    int angle;
    if(rand()%2){
        pAsteroid->x=rand()%pAsteroid->window_width-pAsteroid->rect.w/2;
        pAsteroid->y=-pAsteroid->rect.h;
        angle=rand()%90-45;
    }else{
        pAsteroid->y=rand()%pAsteroid->window_height-pAsteroid->rect.h/2;
        pAsteroid->x=-pAsteroid->rect.w;
        angle=rand()%90;
    }
    int v=rand()%8+5;
    pAsteroid->vx=v*sin(angle*2*M_PI/360);
    pAsteroid->vy=v*cos(angle*2*M_PI/360);
    pAsteroid->rect.x=pAsteroid->x;
    pAsteroid->rect.y=pAsteroid->y;
}
*/


/*
void getBulletSendData(Bullet *pBullet,BulletData *pBulletData){
    pBulletData->time = pBullet->time;
    pBulletData->vx = pBullet->vx;
    pBulletData->vy = pBullet->vy;
    pBulletData->x = pBullet->x;
    pBulletData->y = pBullet->y;
}

void updateBulletWithRecievedData(Bullet *pBullet,BulletData *pBulletData){
    pBullet->time = pBulletData->time;
    pBullet->vx = pBulletData->vx;
    pBullet->vy = pBulletData->vy;
    pBullet->x = pBulletData->x;
    pBullet->y = pBulletData->y;
}
*/

