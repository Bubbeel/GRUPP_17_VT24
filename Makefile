# Makefile for Windows
SRCDIR=./source
CC=gcc
INCLUDE = C:\msys64\mingw64\include\SDL2 
#, C:\msys64\var\lib\pacman\local\mingw-w64-x86_64-SDL2_ttf-2.20.2-1

CFLAGS = -g -I$(INCLUDE) -c 
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -lm

CTFGame: main.o gridMap.o
	gcc -o CTFGame main.o gridMap.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

gridMap.o: $(SRCDIR)/gridMap.c
	gcc -c $(SRCDIR)/gridMap.c

clean:
	rm *.exe
	rm *.o
