#ifndef MUSIC_H
#define MUSIC_H

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


static Mix_Music *gameMusic = NULL;
static Mix_Chunk *playerWalkSound = NULL;
static Mix_Chunk *playerShootSound = NULL;
static Mix_Chunk *playerPickUpItemSound = NULL;
static Mix_Chunk *playerGotFlagSound = NULL;
static Mix_Chunk *menuClickSound = NULL;
static Mix_Chunk *playerHurtSound = NULL;
static Mix_Chunk *playerGotPointSound = NULL;
static Mix_Chunk *playerWinningSound = NULL;
static Mix_Chunk *gameOverSound = NULL;
static Mix_Chunk *playerDiedSound = NULL;

int initAudio();
void switchThemeSong();
void playWalkingSound();
void playPlayerPickUpItemSound();
void playPlayerGotFlagSound();
void playMenuClickSound();
void playPlayerGotPointSound();
void playShootSound();
void playHurtSound();
void playWinningSound();
void playGameIsOverSound();
void playPlayerDiedSound();
void closeAudio();

#endif