#ifndef CHESS_H 
#define CHESS_H

#include <map>
#include <stack>

#include "Board.h"
#include "Player.h"

// Result for a move - player is left in checkmate, stalemate, or neither.
enum Result 
    {
    None, 
    Checkmate, 
    Stalemate,
    };


// Stores the game state (as well as the board state)
struct GameState 
    {
    BoardState boardState;
    u8 winner;
    Result endResult;
    bool whiteMove;
    }; 

// Manages a chess game. Add the players and then call tick() to make moves.
// No time buffering is done in tick(), this is up to the callee.
struct Chess 
    {
    Chess (Player *white, Player *black);

    // Returns the board.
    auto board () -> const Board& { return m_board; }

    // Undoes a move.
    auto undo () -> void;

    // Perform a move. 
    auto tick () -> Result;

    Result endResult = None;
    u8 winner = 0;
protected: 
    Board m_board;
    std::map <u8, Player*> m_players;
    bool m_whiteMove = true;

    std::stack <GameState> m_prevStates, m_nextStates;

    // Takes a snapshot of the current board state. 
    auto snapshot () -> GameState;

    // Loads the specified state.
    auto loadState (const GameState& state) -> void; 

    // Creates a default board.
    auto createDefaultBoard () -> void;
    };


#endif 