#include <map>
#include <iostream>
#include <functional>
#include <algorithm>
#include <chrono>
#include "Canvas.h"
#include "Runner.h"
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
                { "clearpath",      makeClearPath },
                { "centre",         makeCentre },
                { "aggressive",     makeAggresive },
                { "passive",        makePassive },
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
    if (args.size() <= 2)
        {
        std::cerr << "usage: " << args[0] << " white black" << std::endl;
        return EXIT_FAILURE;
        }
    

    Viewer viewer { "chessview", 400, 400 };
    Runner runner { &viewer };
    runner.addPlayer ( WHITE, makePlayer ( args.at(1) ));
    runner.addPlayer ( BLACK, makePlayer ( args.at(2) ));
    for (;;)
        {
        SDL_Event e; 
        while ( SDL_PollEvent(&e) )
            {
            switch (e.type) 
                {
                case SDL_QUIT: // Exit
                    return EXIT_SUCCESS;
                }
            }
        if ( !runner.tick() )
            break;
        }
    return EXIT_SUCCESS;
    }