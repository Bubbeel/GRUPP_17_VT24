SRCDIR=./source
CC=gcc
INCLUDE = -I"C:\SDL2\include"
LIBS = -L"C:\SDL2\lib"
CFLAGS = -g $(INCLUDE) -c
LDFLAGS = $(LIBS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lm

CTFGame.exe: main.o player.o flag.o gridMap.o collisionDetection.o serverClient.o menu.o
	$(CC) -o CTFGame.exe main.o player.o flag.o gridMap.o collisionDetection.o serverClient.o menu.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

player.o: $(SRCDIR)/player.c
	$(CC) $(CFLAGS) $(SRCDIR)/player.c

flag.o: $(SRCDIR)/flag.c
	$(CC) $(CFLAGS) $(SRCDIR)/flag.c

gridMap.o: $(SRCDIR)/gridmap.c
	$(CC) $(CFLAGS) $(SRCDIR)/gridmap.c

collisionDetection.o: $(SRCDIR)/collisionDetection.c
	$(CC) $(CFLAGS) $(SRCDIR)/collisionDetection.c

serverClient.o: $(SRCDIR)/serverClient.c
	$(CC) $(CFLAGS) $(SRCDIR)/serverClient.c

menu.o: $(SRCDIR)/menu.c
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c

clean:
	del *.exe
	del *.o
