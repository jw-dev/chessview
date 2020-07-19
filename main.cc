#include <iostream>
#include <algorithm>
#include "Runner.h"
#include "Player.h"
#include "Tournament.h"

using PlayerCreator = std::function<std::unique_ptr<Player>()>;

// List of available players
static const std::map <std::string, PlayerCreator>
    players = {
                { "random",         makeRandom },
                { "whitesquares",   makeWhiteSquares },
                { "blacksquares",   makeBlackSquares },
                { "min",            makeMinimizeOpponentMoves },
                { "max",            makeMaximizeOpponentMoves },
                { "min_self",       makeMinimizeOwnMoves },
                { "max_self",       makeMaximizeOwnMoves },
                { "defensive",      makeDefensive },
                { "offensive",      makeOffensive },
                { "suicidal",       makeSuicidal },
                { "pacifist",       makePacifist },
                { "edge",           makeClearPath },
                { "centre",         makeCentre },
                { "aggressive",     makeAggresive },
                { "passive",        makePassive },
                { "expensive",      makeExpensive },
                { "cheap",          makeCheap },
                { "far",            makeFar },
                { "near",           makeNear },
              };

// Helper function to create a player from their name
auto makePlayer (const std::string& arg) -> std::unique_ptr<Player>
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

struct Options 
    {
    bool isValid, isNoUi, isTournament;
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
        static const auto find = 
            [&args] (const std::string& match) -> bool
                {
                return std::find(args.begin(), args.end(), match) != args.end();
                };
        opt.isNoUi = find("-noui");
        opt.isTournament = find("-tournament");
        opt.whiteName = args.at (args.size() - 2 );
        opt.blackName = args.at (args.size() - 1 );
        }
    return opt;
    }

auto getRunner (const Options& opt) -> std::unique_ptr<Runner> 
    {
    std::unique_ptr<Player> white = makePlayer ( opt.whiteName ),
                            black = makePlayer ( opt.blackName );
    switch (opt.isNoUi)
        {
        case true: return std::make_unique<RunnerStd> (std::move (white), std::move (black));
        case false: return std::make_unique<RunnerUI> (std::move (white), std::move (black));
        }
    }

int main (int argc, char ** argv)
    {
    Options opt = parseOpt ( argc, argv );
    if (!opt.isValid) 
        {
        std::cerr << "usage: " << argv[0] << " [-noui] white black" << std::endl;
        return EXIT_FAILURE;
        }

    if (opt.isTournament) 
        {
        Tournament t { players };
        t.run();
        return EXIT_SUCCESS;
        }

    auto runner = getRunner (opt);
    if (runner)
        {
        runner->run();
        }
    }