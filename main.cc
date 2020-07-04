
#include "Chess.h"
#include "Draw.h"
#include "Player.h"

int main (void) 
    {
    Chess chessGame ( makeRandom(), makeMinimizeOpponentMoves() );
    Draw draw { chessGame.board(), "chessview", 400, 400 };

    bool quit = false;
    while (!quit)
        {
        if (chessGame.endResult == None)
            {
            chessGame.tick();
            }
        quit = draw.draw();
        }
    return EXIT_SUCCESS;
    }