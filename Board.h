#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "Move.h"

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

// Holds board state. 
// Board state is held in m_pieces. 
struct Board 
    {
    // Length of the grid. Grid lengths of different sizes (up to 16!) should work, but untested.
    const static u8 GRID_LENGTH = 8;
    // Masks
    const static u8 PIECE_MASK = 0b1111;
    const static u8 TYPE_MASK = 0b0111;
    const static u8 COLOR_MASK = 0b1000;
    // Bits 
    const static u16 WHITE_KINGMOVE_MASK = 0b01;
    const static u16 BLACK_KINGMOVE_MASK = 0b10;
    const static u16 STALE_MASK = 0b0111111100;
    const static u16 WHITEMOVE_MASK = 0b1000000000;

    Move lastMove = {0};

    Board ();
    Board (const Board& other);
    auto operator=(const Board& other) =delete;

    // Resets the board so there are no pieces, and all flags reset.
    auto reset() -> void;

    // Returns the piece encoded as a u8 at the specified column and row.
    // Bitwise magic with PieceBits is required to extract information.
    auto pieceAt (u8 column, u8 row) const -> u8;

    // Returns the colour of the specified tile. (WHITE or BLACK)
    auto colorAt (u8 column, u8 row) const -> u8;

    // Sets the piece at the specified position to the specified piece. 
    // piece must bitwise AND with exactly one type and one colour; otherwise, the behaviour is undefined.
    auto setPiece (u8 piece, u8 column, u8 row) -> void;

    // Blanks out a piece at the specified position (set to 0).
    auto removePiece (u8 column, u8 row) -> void;

    // Checks whether the move is legal. Sanity checks, checks whether the piece can move as desired, checks checkmate conditions, etc.
    auto isMoveLegal (const Move& move) -> bool;

    // Performs a move. Calls isMoveLegal() first to ensure the move is legal.
    auto doMove (const Move& move) -> bool;

    // Returns the number of moves for the specified player (color) WHITE or BLACK.
    // Specify count as non-zero to restrict the result to [count] moves only. For example, count = 1 returns exactly one move (or zero if none legal).
    auto getMoves (u8 count = 0) -> std::vector<Move>;

    // Returns whether the specified player (WHITE or BLACK) has exactly zero moves remaining. (For checkmate and stalemate situations)
    auto hasZeroMoves () -> bool;

    // Returns whether the specified player has exactly one move remaining. (For stalemate situations).
    auto hasOneMove () -> bool;

    // Returns whether the specified player is currently in check (King is attacked by a piece).
    auto isCheck (u8 color) -> bool;

    // Returns whether the tile is attacked by an opponent's piece. 
    // Note we don't specify a player here; the piece at the target is taken as the defender.
    auto isAttacked (u8 column, u8 row) const -> bool;

    // Same as above, but specifying a piece that, if it were on that square, would be attacked.
    auto isAttacked (u8 column, u8 row, u8 piece) const -> bool;

    // Returns true if the move is an en passant capture.
    auto isEnPassant (const Move& move) const -> bool; 
    
    // Returns the current board state, depending on whose turn it is.
    auto getBoardState () -> BoardState;

    // Returns true if the specified player has moved their king. (For castling rules.)
    auto kingMoved ( u8 player ) const -> bool;

    // Returns true if it is white's move, otherwise false.
    auto whiteMove () const -> bool;

    // Returns the number of full moves in total. 
    auto fullMoveClock () const -> u16;

    // Returns the number of half-moves since a capture or pawn advance.
    auto staleHalfMoveClock () const -> u16;

    // Try a move and then revert the board state.
    // The function f() is a user-specified function to check the board state.
    template <class T>
    auto tryMove (const Move& move, const T &func) -> decltype (func()) 
        {
        u32 src = m_pieces [move.fromRow];
        u32 dest = m_pieces [move.toRow];
        u16 bits = m_bits;
        u16 halfMoves = m_halfMoves;
        Move _lastMove = lastMove;
        forceDoMove (move);
        auto result = func();
        m_pieces [move.fromRow] = src;
        m_pieces [move.toRow] = dest;
        m_bits = bits;
        m_halfMoves = halfMoves;
        lastMove = _lastMove;
        return result;
        }

protected:
    // Board pieces. 
    // Each piece is stored in a u8, but actually only takes up 4 bits. 
    // Since a chess grid is 8x8, this means we can encode each row as 4 bits x 8 bits = 32 bits - a single u32.
    // So, m_pieces is 8 u32s, one for each row on the board.
    std::vector <u32> m_pieces;

    // Board state bits.
    // Bit 0 is used for when the white King has moved.
    // Bit 1 is used for when the black King has moved.
    // Bits 2-8 are used for the "stale move" counter.
    // Bit 9 is used for whether or not it is white's move (true is white move.)
    // Bits 10-16 are reserved. 
    u16 m_bits = WHITEMOVE_MASK;

    // Half-move counter. 
    // Required for FEN export.
    u16 m_halfMoves = 0U;

    // Do a move without checking if it is legal, just do it.
    auto forceDoMove (const Move& move) -> void;

    // Checks whether the specified column and row is within the confines of the board.
    auto inBounds (u8 column, u8 row) const -> bool; 

    // Checks whether the source tile and target tile of the move is within the confines of the board.
    auto inBounds (const Move& move) const -> bool;

    // Checks whether the piece can move in the manner specified. For example, no pawns moving 3 tiles, no kings moving 2 tiles, etc.
    auto isMoveLegalForPiece (u8 piece, const Move& move) -> bool;

    // Returns whether the specified move would leave the player in check. This is an illegal move.
    auto isMoveIntoCheck (const Move& move) -> bool;
    };


#endif 