# Makefile for Windows
SRCDIR=./source
CC=gcc
INCLUDE = C:\msys64\mingw64\include\SDL2 
CFLAGS = -g -I$(INCLUDE) -c
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -lSDL2_net -lm

CTFGame.exe: main.o player.o flag.o gridMap.o collisionDetection.o client.o server.o menu.o
	$(CC) -o CTFGame.exe main.o player.o flag.o gridMap.o collisionDetection.o client.o server.o menu.o $(LDFLAGS)

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

client.o: $(SRCDIR)/client.c
	$(CC) $(CFLAGS) $(SRCDIR)/client.c

server.o: $(SRCDIR)/server.c
	$(CC) $(CFLAGS) $(SRCDIR)/server.c

menu.o: $(SRCDIR)/menu.c
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c

clean:
	del CTFGame.exe
	del *.o
