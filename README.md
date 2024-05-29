# Grupp 17, Flaggy Labby - Multiplayer Game using SDL2 and C


## Table of Contents
- [Description](#description)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)

## Description
4-player multiplayer game, developed using SDL2 and C. In which your object is to navigate yourself through the maze, find the flag and capture it in order to win. Beware, as other players will fight you over death for the flag!
### Trailer:
[![FlaggyLabbyTrailer](https://img.youtube.com/vi/yn_OVyW2HdY/0.jpg)](https://www.youtube.com/watch?v=yn_OVyW2HdY)

## Features
- Multiplayer functionality: Play with up-to 3 of your friends and capture the flag.
- PvP: Attack your friends in order to  re-take the flag, but beware of them as well.
- Variety of items: Pick up a variety of items throughout the map and power yourself up!
- Competitive gameplay: First person to get 3 points, wins!

## Requirements
- C compiler (Msys2 was used for this project)
- SDL2 library

## Installation
1. Clone the repository:

   ```bash
   git clone https://github.com/Bubbeel/GRUPP_17_VT24.git
   ```

3. Build the game:
   ### Windows:
   ```powershell
   mingw32-make.exe all
   ```
   ### Linux:
   ```bash
   make all
   ```
   ### MacOS:
   ```terminal
   make all
   ```

## Usage
### 0. Enter network settings in (../lib/source/serverClient.c).
   Enter in IP and/or port. Within mainClient.c, line 135, set host server's IP address.
### 1. Join Host Server
   ```bash
   ipconfig
   ```
### 3. Compile Game:
   ```bash
   mingw32-make.exe
   ```
### 4. Set Up Server
   ```bash
   .\CTFServer.exe
   ```
### 5. 4 Players/Clients Join Host Server
   ```bash
   .\CTFClient.exe
   ```
### 5. Play the game.
   ### Lobby:
   Once youve connected to the server, press the start button and wait until all other players have also.
   ### Controls:
   You move your character using W A S D, and space to shoot. Weapons and flag auto-pick up
### 6. Enjoy the multiplayer gaming experience with your friends!
   Have fun and play with friends!
