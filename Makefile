# Makefile for MacOS
SRCDIR=./source
CC=gcc
INCLUDE = -I/usr/local/include/SDL2
LIBS = -L/usr/local/lib
CFLAGS = -g $(INCLUDE) -c 
LDFLAGS = $(LIBS) -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -lm

CTFGame: main.o player.o flag.o gridMap.o
	$(CC) -o CTFGame main.o player.o flag.o gridMap.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

player.o: $(SRCDIR)/objects/player.c
	$(CC) $(CFLAGS) $(SRCDIR)/objects/player.c

flag.o: $(SRCDIR)/objects/flag.c
	$(CC) $(CFLAGS) $(SRCDIR)/objects/flag.c

gridMap.o: $(SRCDIR)/gridMap.c
	$(CC) $(CFLAGS) $(SRCDIR)/gridMap.c
clean:
	rm *.exe
	rm *.o
