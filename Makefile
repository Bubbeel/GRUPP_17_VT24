# Makefile for macOS
SRCDIR=./source
CC=gcc
INCLUDE = C:\msys64\mingw64\include\SDL2 
#, C:\msys64\var\lib\pacman\local\mingw-w64-x86_64-SDL2_ttf-2.20.2-1

CFLAGS = -g -I$(INCLUDE) -c 
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -mwindows -lm

simpleSDLexample1: main.o player.o flag.o
	$(CC) main.o player.o flag.o -o simpleSDLexample1 $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

player.o: $(SRCDIR)/objects/player.c
	$(CC) $(CFLAGS) $(SRCDIR)/objects/player.c

flag.o: $(SRCDIR)/objects/flag.c
	$(CC) $(CFLAGS) $(SRCDIR)/objects/flag.c

clean:
	rm -f *.o
