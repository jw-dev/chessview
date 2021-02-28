#ifndef FEN_H
#define FEN_H 

#include "Board.h"
#include <sstream>

namespace FEN 
    {
    auto toFEN ( Board* board, u16 halfMoveStaleClock, u16 fullMoveclock ) -> std::string;
    }

#endif