# Makefile for macOS
SRCDIR=./source
CC=gcc
INCLUDE = -I/usr/local/include/SDL2
LIBS = -L/usr/local/lib
CFLAGS = -g $(INCLUDE) -c 
LDFLAGS = $(LIBS) -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -lSDL2_net -lm

CTFGame: main.o player.o flag.o gridMap.o collisionDetection.o serverClient.o menu.o
	gcc -o CTFGame main.o player.o flag.o gridMap.o collisionDetection.o serverClient.o menu.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

player.o: $(SRCDIR)/player.c
	$(CC) $(CFLAGS) $(SRCDIR)/player.c

flag.o: $(SRCDIR)/flag.c
	$(CC) $(CFLAGS) $(SRCDIR)/flag.c

gridMap.o: $(SRCDIR)/gridMap.c
	$(CC) $(CFLAGS) $(SRCDIR)/gridMap.c

collisionDetection.o: $(SRCDIR)/collisionDetection.c
	$(CC) $(CFLAGS) $(SRCDIR)/collisionDetection.c

serverClient.o: $(SRCDIR)/serverClient.c
	$(CC) $(CFLAGS) $(SRCDIR)/serverClient.c

menu.o: $(SRCDIR)/menu.c
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c

clean:
	rm *.exe
	rm *.o
