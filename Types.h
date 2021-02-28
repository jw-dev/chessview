#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

using u8 = uint_least8_t;
using u16 = uint_least16_t;
using u32 = uint_least32_t;

// Enum to hold information about a piece in 4 bits.
// For example, this lets us use ROOK | WHITE (a white rook), which would be 0b1110 (14).
enum PieceBits
    {
    EMPTY = 0,
    BLACK = 0,
    PAWN = 1,
    BISHOP = 2,
    KNIGHT = 3,
    QUEEN = 4,
    KING = 5,
    ROOK = 6,
    CASTLE = 7,
    WHITE = 8, 
    };

// Board state. 
enum BoardState 
    {
    STATE_NORMAL,
    STATE_CHECKMATE,
    STATE_STALEMATE,
    STATE_FORCED_DRAW_INSUFFICIENT_MATERIAL,
    STATE_FORCED_DRAW_FIFTY_MOVES,
    };

// Length of the grid. Grid lengths of different sizes (up to 16!) should work, but untested.
const static u8 GRID_LENGTH = 8;
// Masks
const static u8 PIECE_MASK = 0b1111;
const static u8 TYPE_MASK = 0b0111;
const static u8 COLOR_MASK = 0b1000;
// Bits 
const static u8 BLACKMOVE_MASK = 0b000001;
const static u8 DOUBLE_MASK = 0b000010;
const static u8 PAWN_MASK = 0b011100;
const static u8 STALE_MASK = 0b100000;

#endif