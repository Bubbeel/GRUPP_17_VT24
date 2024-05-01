SRCDIR=./source
CC=gcc
INCLUDE = -I/usr/local/include/SDL2
LIBS = -L/usr/local/lib
CFLAGS = -g $(INCLUDE) -c 
LDFLAGS = $(LIBS) -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -lm -lSDL2_net

CTFGame: main.o player.o flag.o gridMap.o client.o server.o
	$(CC) -o CTFGame main.o player.o flag.o gridMap.o client.o server.o $(LDFLAGS)

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

clean:
	rm *.exe
	rm *.o
