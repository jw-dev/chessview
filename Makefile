CC=g++
CFLAGS=-Ofast -march=native -pipe -Wall -Wextra -Wfatal-errors -pedantic -Weffc++ -pedantic-errors -g
DEPENDS=-lSDL2 -lSDL2_image
OUT=chess

all: Board.cc Viewer.cc Runner.cc main.cc Player.cc RunnerStd.cc RunnerUI.cc
	$(CC) $(DEPENDS) $(CFLAGS) Board.cc Viewer.cc Runner.cc main.cc Player.cc RunnerStd.cc RunnerUI.cc -o $(OUT)