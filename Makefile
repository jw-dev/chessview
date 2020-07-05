CC=g++
CFLAGS=-Ofast -march=native -pipe -Wall -Wextra -Wfatal-errors -pedantic -Weffc++ -pedantic-errors
DEPENDS=-lSDL2 -lSDL2_image
OUT=chess

all: Board.cc Chess.cc Draw.cc main.cc Player.cc
	$(CC) $(DEPENDS) $(CFLAGS) Board.cc Chess.cc Draw.cc main.cc Player.cc -o $(OUT)