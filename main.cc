#include <map>
#include <iostream>
#include <functional>
#include "Chess.h"
#include "Draw.h"
#include "Player.h"

static const std::map <std::string, std::function<Player*()>>
    players = {
                { "random",         makeRandom },
                { "whitesquares",   makeWhiteSquares },
                { "blacksquares",   makeBlackSquares },
                { "minimize",       makeMinimizeOpponentMoves },
                { "defensive",      makeDefensive },
                { "offensive",      makeOffensive },
                { "suicidal",       makeSuicidal },
                { "pacifist",       makePacifist },
              };

auto makePlayer (const std::string& arg) -> Player* 
    {
    if ( players.find (arg) != players.end() )
        {
        return players.at (arg) ();
        }
    else 
        {
        std::cerr << "Unknown player: " << arg << " (assuming random)" << std::endl;
        return makeRandom ();
        }
    }

int main (int argc, char ** argv)
    {
    std::vector<std::string> args (argv, argv + argc);
    if (args.size() == 1) 
        {
        std::cerr << "usage: " << args[0] << " white black" << std::endl;
        return EXIT_FAILURE;
        }

    Chess chessGame ( makePlayer(args.at(1)), makePlayer(args.at(2)) );
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