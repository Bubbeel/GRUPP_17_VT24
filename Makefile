# Makefile for Windows
SRCDIR=./source
CC=gcc
INCLUDE = C:\msys64\mingw64\include\SDL2 

CFLAGS = -g -I$(INCLUDE) -c 
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -lm

CTFGame: main.o player.o flag.o gridMap.o
	gcc -o CTFGame main.o player.o flag.o gridMap.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

player.o: $(SRCDIR)/objects/player.c
	$(CC) $(CFLAGS) $(SRCDIR)/objects/player.c

flag.o: $(SRCDIR)/objects/flag.c
	$(CC) $(CFLAGS) $(SRCDIR)/objects/flag.c

gridMap.o: $(SRCDIR)/gridMap.c
	gcc -c $(SRCDIR)/gridMap.c

clean:
	rm *.exe
	rm *.o
