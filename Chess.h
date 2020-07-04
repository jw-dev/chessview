#ifndef CHESS_H 
#define CHESS_H

#include <map>

#include "Board.h"
#include "Player.h"

// Result for a move - player is left in checkmate, stalemate, or neither.
enum Result 
    {
    None, 
    Checkmate, 
    Stalemate,
    };

// Manages a chess game. Add the players and then call tick() to make moves.
// No time buffering is done in tick(), this is up to the callee.
struct Chess 
    {
    Chess (Player *white, Player *black);

    // Returns the board.
    auto board () -> const Board& { return m_board; }

    // Perform a move. 
    auto tick () -> Result;

    Result endResult = None;
    u8 winner = 0;
protected: 
    Board m_board;
    std::map <u8, Player*> m_players;
    bool m_whiteMove;

    auto createDefaultBoard () -> void;
    };


#endif 