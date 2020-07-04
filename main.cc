
#include "Chess.h"
#include "Draw.h"
#include "Player.h"

#include <stack>

int main (void) 
    {
    Chess chessGame ( makeDefensive(), makeDefensive() );
    Draw draw { chessGame.board(), "chessview", 400, 400 };

    bool play = false;
    for (;;)
        {
        SDL_Event e; 
        while ( SDL_PollEvent(&e) )
            {
            switch (e.type) 
                {
                case SDL_KEYDOWN: 
                    switch (e.key.keysym.sym)
                        {   
                        case SDLK_LEFT: 
                            if (!play)
                                chessGame.undo();
                            break;
                            
                        case SDLK_RIGHT:
                            if (!play && chessGame.endResult == None)
                                chessGame.tick();
                            break;
                        case SDLK_SPACE:
                            play = !play;
                            break;
                        }
                        break;
                case SDL_QUIT: // Exit
                    return EXIT_SUCCESS;
                }
            }

        if (play && chessGame.endResult == None)
            {
            chessGame.tick();
            }
        draw.draw();
        }
    return EXIT_SUCCESS;
    }