#include <iostream>
#include <algorithm>
#include "Runner.h"
#include "Player.h"



using PlayerCreator = std::function<std::unique_ptr<Player>()>;

static std::string programName;

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

static void error ( const std::string& msg ) 
    {
    std::cerr << msg 
              << std::endl 
              << "usage: " << programName << " [-headless] white black" 
              << std::endl;
    }

// Helper function to create a player from their name
static auto makePlayer (const std::string& arg) -> std::unique_ptr<Player>
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


static std::size_t numPlayers ( const std::vector<std::string>& args )
    {
    std::size_t players = 0;

    for ( int i = 1; i < args.size(); ++i )
        {
        const std::string& arg = args.at ( i );
        if ( arg[0] == '-' ) continue;
        players++;
        }
    return players;
    }

static bool findArg ( const std::vector<std::string>& args, const std::string& needle )
    {
    return std::find ( args.begin(), args.end(), needle ) != args.end();
    }

int main (int argc, char ** argv)
    {
    std::vector<std::string> args (argv, argv + argc);

    programName = argv[0];
    bool isHeadless = findArg ( args, "-headless" );
    int players = numPlayers ( args );
    std::unique_ptr <Runner> runner;

    switch ( players ) 
        {
        case 1: 
            {
            if ( isHeadless ) 
                error ( "headless requires two players to be provided" );
            auto white = makePlayer ( args.back() );
            runner = std::make_unique<RunnerUI> ( std::move( white ) );
            break;
            }
        case 2:
            {
            auto black = makePlayer ( args.back() );
            auto white = makePlayer ( args.at ( args.size() - 2 ) );
            if ( isHeadless ) runner = std::make_unique<RunnerStd> ( std::move( white ), std::move( black ) ); 
            else              runner = std::make_unique<RunnerUI> ( std::move( white ), std::move( black ) ); 
            break;
            }
        default: 
            {
            if ( !players ) error ( "one or more players must be provided" );
            else            error ( "unexpected argument" );
            return EXIT_FAILURE;
            }
        }

    if ( runner )
        {
        runner->run ();
        }
    }