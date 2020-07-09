#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "Move.h"

using u8 = uint_least8_t;
using u32 = uint_least32_t;

// Enum to hold information about a piece in 4 bits.
// For example, this lets us use ROOK | WHITE (a white rook), which would be 0b1110 (14).
enum PieceBits
    {
    NONE = 0,
    BLACK = 0,
    PAWN = 1,
    BISHOP = 2,
    KNIGHT = 3,
    QUEEN = 4,
    KING = 5,
    ROOK = 6,
    WHITE = 8, 
    };

// Structure used for preserving board state via getState() and setState().
struct BoardState 
    {
    std::vector <u32> pieces;
    u32 whiteStaleMoves = 0, blackStaleMoves = 0;
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
    // For when a tile has no piece in it
    const static u8 EMPTY = 0;

    Move lastMove = {0};

    Board ();
    Board (const Board& other) = delete;

    // Returns the current board state.
    auto getState () const -> BoardState;

    // Sets the current board state.
    auto setState (const BoardState& state) -> void;

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
    auto getMoves (u8 color, u8 count = 0) -> std::vector<Move>;

    // Returns whether the specified player (WHITE or BLACK) has exactly zero moves remaining. (For checkmate and stalemate situations)
    auto hasZeroMoves (u8 color) -> bool;

    // Returns whether the specified player has exactly one move remaining. (For stalemate situations).
    auto hasOneMove (u8 color) -> bool;

    // Returns whether the specified player is currently in check (King is attacked by a piece).
    auto isCheck (u8 color) -> bool;

    // Returns whether the specified player is currently in checkmate (King is attacked and zero moves).
    auto isCheckmate (u8 color) -> bool;

    // Returns whether the specified player is in a stalemate situation (Zero moves, stale moves, or insufficient material for checkmate).
    auto isStalemate (u8 color) -> bool;

    // Returns whether the tile is attacked by an opponent's piece. 
    // Note we don't specify a player here; the piece at the target is taken as the defender.
    auto isAttacked (u8 column, u8 row) const -> bool;

    

    // Try a move and then revert the board state.
    // The function f() is a user-specified function to check the board state.
    template <class T>
    auto tryMove (const Move& move, const T &func) -> decltype (func()) 
        {
        u32 src = m_pieces [move.fromRow];
        u32 dest = m_pieces [move.toRow];
        u32 whiteStaleMoves = m_whiteStaleMoves;
        u32 blackStaleMoves = m_blackStaleMoves;
        forceDoMove (move);
        auto result = func();
        m_pieces [move.fromRow] = src;
        m_pieces [move.toRow] = dest;
        m_whiteStaleMoves = whiteStaleMoves;
        m_blackStaleMoves = blackStaleMoves;
        return result;
        }

protected:
    // Board pieces. 
    // Each piece is stored in a u8, but actually only takes up 4 bits. 
    // Since a chess grid is 8x8, this means we can encode each row as 4 bits x 8 bits = 32 bits - a single u32.
    // So, m_pieces is 8 u32s, one for each row on the board.
    std::vector <u32> m_pieces;

    // Tracking of "stale" moves (no pawn move, no capture for x turns).
    // Stalemate is a situation where both these values are >= 50.
    u32 m_whiteStaleMoves = 0, m_blackStaleMoves = 0;

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