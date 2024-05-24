#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include "music.h"

int initAudio() {


    gameMusic = Mix_LoadMUS("../lib/resources/audio/menuTheme_song.mp3"); // main theme song//
    if (!gameMusic) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load OGG file: %s", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeMusic(MIX_MAX_VOLUME / 8);
    Mix_PlayMusic(gameMusic, -1);


    playerWalkSound = Mix_LoadWAV("../lib/resources/audio/walk_sound.mp3"); // walk sound
    if (playerWalkSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(playerWalkSound, MIX_MAX_VOLUME * 0.02);


    playerPickUpItemSound = Mix_LoadWAV("../lib/resources/audio/pickUpFlag.wav"); // picking up the flag sound
    if (playerPickUpItemSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(playerPickUpItemSound, 20);


    playerGotFlagSound = Mix_LoadWAV("../lib/resources/audio/flagTaken_sound.wav"); // send to all clients a player has taken the flag
    if (playerGotFlagSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(playerGotFlagSound, 20);


    menuClickSound = Mix_LoadWAV("../lib/resources/audio/menu_click_sound.wav"); // menu click sound
    if (menuClickSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(menuClickSound, 40);


    playerGotPointSound = Mix_LoadWAV("../lib/resources/audio/flagPoint_sound.wav"); // player deliver flag to spawnPoint
    if (playerGotPointSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(playerGotPointSound, 20);


    playerShootSound = Mix_LoadWAV("../lib/resources/audio/shot_sound.wav"); // bullet sound
    if (playerShootSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(playerShootSound, 20);


    playerHurtSound = Mix_LoadWAV("../lib/resources/audio/hurt_sound.wav"); // player taking damage
    if (playerHurtSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(playerHurtSound, 20);


    playerWinningSound = Mix_LoadWAV("../lib/resources/audio/winning_sound.wav"); // player delivered flag and won the game
    if (playerWinningSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(playerWinningSound, 20);


    gameOverSound = Mix_LoadWAV("../lib/resources/audio/game_over_sound.wav"); // one player delivered the flag, game is over. send to all clients.
    if (gameOverSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(gameOverSound, 20);


    playerDiedSound = Mix_LoadWAV("../lib/resources/audio/player_dead_sound.wav"); // one player delivered the flag, game is over. send to all clients.
    if (playerDiedSound == NULL) {
        printf("%s\n", Mix_GetError());
        closeAudio();
        return 0;
    }
    Mix_VolumeChunk(playerDiedSound, 20);



    return 1;
}


// audio functions
void playWalkingSound() {
    static int timeInterval;
    int currentTime;
    currentTime = SDL_GetTicks() - timeInterval;
    if (currentTime > 500) {
        Mix_PlayChannelTimed(-1, playerWalkSound, 0, 500);
        timeInterval = SDL_GetTicks();
    }
}

void switchThemeSong() {
    static bool menuPlaying = true;
    Mix_FreeMusic(gameMusic);
    if (menuPlaying) {
        gameMusic = Mix_LoadMUS("../lib/resources/audio/mainTheme_song.mp3");
        if (!gameMusic) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load lobby music mp3 file: %s", Mix_GetError());
            closeAudio();
            return;
        }
        menuPlaying = false;
    }
    else {
        gameMusic = Mix_LoadMUS("../lib/resources/audio/menuTheme_song.mp3");
        if (!gameMusic) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load mainTheme music mp3 file: %s", Mix_GetError());
            closeAudio();
            return;
        }
        menuPlaying = true;
    }
    Mix_PlayMusic(gameMusic, -1);
}

// function calls and play audio
void playPlayerPickUpItemSound() {
    Mix_PlayChannel(-1, playerPickUpItemSound, 0);
}

void playPlayerGotFlagSound() {
    Mix_PlayChannel(-1, playerGotFlagSound, 0);
    /*
    static int timeInterval;
    int CurrentTime;
    CurrentTime = SDL_GetTicks() - timeInterval;
    if (CurrentTime > 10000) {
        Mix_PlayChannelTimed(-1, playerGotFlagSound, 0, 10000);
        timeInterval = SDL_GetTicks();
    }
    */
}


void playMenuClickSound() {
    Mix_PlayChannel(-1, menuClickSound, 0);
}

void playPlayerGotPointSound() {
    Mix_PlayChannel(-1, playerGotPointSound, 0);
}

void playShootSound() {
    Mix_PlayChannel(-1, playerShootSound, 0);
}

void playHurtSound() {
    Mix_PlayChannel(-1, playerHurtSound, 0);
}

void playWinningSound() {
    Mix_PlayChannel(-1, playerWinningSound, 0);
}

void playGameIsOverSound() {
    Mix_PlayChannel(-1, gameOverSound, 0);
}

void playPlayerDiedSound() {
    Mix_PlayChannel(-1, playerDiedSound, 0);
}


//close Audio and clear resources
void closeAudio(){

    if (gameMusic) {
        Mix_FreeMusic(gameMusic);
        gameMusic = NULL;
    }

    if (playerWalkSound) {
        Mix_FreeChunk(playerWalkSound);
        playerWalkSound = NULL;
    }

    if (playerPickUpItemSound) {
        Mix_FreeChunk(playerPickUpItemSound);
        playerPickUpItemSound = NULL;
    }

    if (playerGotFlagSound) {
        Mix_FreeChunk(playerGotFlagSound);
        playerGotFlagSound = NULL;
    }

    if (menuClickSound) {
        Mix_FreeChunk(menuClickSound);
        menuClickSound = NULL;
    }

    if (playerGotPointSound) {
        Mix_FreeChunk(playerGotPointSound);
        playerGotPointSound = NULL;
    }

    if (playerShootSound) {
        Mix_FreeChunk(playerShootSound);
        playerShootSound = NULL;
    }

    if (playerHurtSound) {
        Mix_FreeChunk(playerHurtSound);
        playerHurtSound = NULL;
    }

    if (playerWinningSound) {
        Mix_FreeChunk(playerWinningSound);
        playerWinningSound = NULL;
    }

    if (gameOverSound) {
        Mix_FreeChunk(gameOverSound);
        gameOverSound = NULL;
    }

    if (playerDiedSound) {
        Mix_FreeChunk(playerDiedSound);
        playerDiedSound = NULL;
    }

    Mix_CloseAudio();
}