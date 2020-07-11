#include <map>
#include <iostream>
#include <functional>
#include <algorithm>
#include <chrono>
#include "Runner.h"
#include "Player.h"

struct Options 
    {
    bool isValid = true;
    bool noUi = false;
    std::string whiteName = "";
    std::string blackName = "";
    };

auto parseOpt ( int argc, char ** argv ) -> Options
    {
    Options opt;
    std::vector <std::string> args (argv, argv + argc);
    if (args.size() <= 2) 
        {
        opt.isValid = false;
        }
    else 
        {
        if (std::find(args.begin(), args.end(), "-noui") != args.end())
            {
            opt.noUi = true;
            }
        opt.whiteName = args.at (args.size() - 2 );
        opt.blackName = args.at (args.size() - 1 );
        }
    return opt;
    }


int main (int argc, char ** argv)
    {
    Options opt = parseOpt ( argc, argv );
    if (!opt.isValid) 
        {
        std::cerr << "usage: " << argv[0] << " [-noui] white black" << std::endl;
        return EXIT_FAILURE;
        }

    Viewer* viewer = nullptr; 
    if (!opt.noUi)
        {
        viewer = new Viewer ( "chessview", 400, 400 );
        }
    // Viewer* viewer = noUi? nullptr: new Viewer ( "chessview", 400, 400 );
    Runner runner {};
    runner.addPlayer ( WHITE, makePlayer ( opt.whiteName ));
    runner.addPlayer ( BLACK, makePlayer ( opt.blackName ));

    bool quit = false, paused = false;
    while ( !quit )
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
                            if (paused) 
                                runner.undo ();
                            break;
                        case SDLK_RIGHT:
                            if (paused && runner.gameState == STATE_NORMAL)
                                runner.tick ();
                            break;
                        case SDLK_SPACE:
                            paused = !paused;
                            break;
                        }
                    break;
                case SDL_QUIT: // Exit
                    quit = true;
                    break;
                }
            }
        if ( !paused && runner.gameState == STATE_NORMAL )
            {
            if ( !runner.tick() )
                break;
            }

        if (viewer)
            {
            viewer->draw (runner.board());
            if (!paused)
                SDL_Delay ( 300 );
            }
        }

    if (viewer)
        delete viewer;
    return EXIT_SUCCESS;
    }