#include <iostream>
#include <algorithm>
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

auto getRunner (const Options& opt) -> Runner*
    {
    Player * white = makePlayer (opt.whiteName), * black = makePlayer (opt.blackName);
    if (opt.noUi)
        {
        return new RunnerStd (white, black);
        }
    else 
        {
        return new RunnerUI (white, black);
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

    Runner * runner = getRunner(opt);
    runner->run();
    }