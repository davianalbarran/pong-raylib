CC = cc
CFLAGS = $(shell pkg-config --cflags raylib)
LDFLAGS = $(shell pkg-config --libs raylib)

all: Pong

Pong: pong.c
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

clean:
	rm -f Pong
