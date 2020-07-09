#ifndef MOVE_H 
#define MOVE_H 

#include <cstdint>

using u8 = std::uint_least8_t;

struct Move 
    {
    u8 fromCol = 0U, toCol = 0U;
    u8 fromRow = 0U, toRow = 0U;
    u8 color = 0U;
    u8 promotion = 0U;
    };

#endif