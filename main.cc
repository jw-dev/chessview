#include <map>
#include <iostream>
#include <functional>
#include <algorithm>
#include <chrono>
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

auto benchmark () -> void 
    {
    using namespace std::chrono;
    std::vector <float> all; 

    auto bench = 
        [] (const std::function<void()> &f)
            {
            auto start = high_resolution_clock::now();
            f();
            return ((float) duration_cast<microseconds> (high_resolution_clock::now() - start).count()) / 1000.0f;
            };

    for (int i = 0; i < 1000; ++i) 
        {
        all.push_back ( bench ([]  
            {
            Chess chessGame ( makeRandom(), makeRandom() );
            while (chessGame.endResult == None) 
                chessGame.tick();
            }));
        }

    auto sum = std::accumulate ( all.begin(), all.end(), 0.0f);
    std::cout << "max: " << (*std::max_element ( all.begin(), all.end() )) << "ms" << std::endl
              << "min: " << (*std::min_element ( all.begin(), all.end() )) << "ms" << std::endl
              << "avg: " << sum / ((float)all.size()) << "ms" << std::endl
              << "tot: " << sum << "ms" << std::endl;
    }

int main (int argc, char ** argv)
    {
    std::vector<std::string> args (argv, argv + argc);
    if (std::find(args.begin(), args.end(), "-b") != args.end())
        {
        benchmark();
        return EXIT_SUCCESS;
        }
    else if (args.size() == 1) 
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