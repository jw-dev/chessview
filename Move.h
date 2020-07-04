#ifndef MOVE_H 
#define MOVE_H 

#include <cstdint>

using u8 = std::uint_least8_t;

struct Move 
    {
    u8 fromCol, toCol;
    u8 fromRow, toRow;
    u8 color;
    };

#endif