SRCDIR=./source
#COMMONSRCDIR=../lib/src
INCDIR=./objects
CC=gcc
INCLUDE = -IC:\msys64\mingw64\include\SDL2 -I$(INCDIR)
CFLAGS = -g -I$(INCLUDE) -c
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -lSDL2_net -lm

CTFGame: main.o player.o flag.o gridMap.o collisionDetection.o serverClient.o menu.o
	gcc -o CTFGame main.o player.o flag.o gridMap.o collisionDetection.o serverClient.o menu.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c $(INCDIR)/player.h $(INCDIR)/gridmap.h $(INCDIR)/playerData.h
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

player.o: $(SRCDIR)/player.c
	$(CC) $(CFLAGS) $(SRCDIR)/player.c

flag.o: $(SRCDIR)/flag.c
	$(CC) $(CFLAGS) $(SRCDIR)/flag.c

gridmap.o: $(SRCDIR)/gridmap.c
	$(CC) $(CFLAGS) $(SRCDIR)/gridmap.c

collisionDetection.o: $(SRCDIR)/collisionDetection.c
	$(CC) $(CFLAGS) $(SRCDIR)/collisionDetection.c

serverClient.o: $(SRCDIR)/serverClient.c
	$(CC) $(CFLAGS) $(SRCDIR)/serverClient.c

menu.o: $(SRCDIR)/menu.c
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c

clean:
	rm *.exe
	rm *.o
