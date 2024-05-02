# Makefile for Windows
SRCDIR=./source
CC=gcc
INCLUDE = -IC:/msys64/mingw64/include/SDL2
LIBS = -LC:/msys64/mingw64/lib
CFLAGS = -g $(INCLUDE) -c 
LDFLAGS = $(LIBS) -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -lSDL2_net -lm

CTFGame.exe: main.o player.o flag.o gridMap.o collisionDetection.o client.o server.o
	$(CC) -o CTFGame.exe main.o player.o flag.o gridMap.o collisionDetection.o client.o server.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

player.o: $(SRCDIR)/player.c
	$(CC) $(CFLAGS) $(SRCDIR)/player.c

flag.o: $(SRCDIR)/flag.c
	$(CC) $(CFLAGS) $(SRCDIR)/flag.c

gridMap.o: $(SRCDIR)/gridMap.c
	$(CC) $(CFLAGS) $(SRCDIR)/gridMap.c

client.o: $(SRCDIR)/client.c
	$(CC) $(CFLAGS) $(SRCDIR)/client.c

server.o: $(SRCDIR)/server.c
	$(CC) $(CFLAGS) $(SRCDIR)/server.c

collisionDetection.o: $(SRCDIR)/collisionDetection.c
	$(CC) $(CFLAGS) $(SRCDIR)/collisionDetection.c

clean:
	del CTFGame.exe
	del *.o
