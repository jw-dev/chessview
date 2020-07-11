#ifndef RUNNER_H
#define RUNNER_H

#include <unordered_map>
#include <sstream>
#include <stack>
#include <assert.h>

#include "Board.h"
#include "Viewer.h"
#include "Player.h"
#include "StateMgr.h"

enum GameState 
    {
    STATE_NORMAL,
    STATE_CHECKMATE,
    STATE_STALEMATE_NO_MOVES,
    STATE_STALEMATE_INSUFFICIENT_MATERIAL,
    STATE_STALEMATE_STALE_MOVES,
    };

struct Runner 
    {
    const u8 TILE_SIZE = 8U;

    GameState gameState;
    u8 winner;

    Runner (Player* white, Player* black);
    Runner (const Runner& other) = delete;
    bool operator=(const Runner& other) = delete;
    ~Runner ();

    auto board () -> const Board&;
    auto undo () -> void; // Undo the last move 
    auto tick () -> bool;

protected:
    std::unordered_map <u8, Player*> m_players;
    bool m_whiteMove;
    StateMgr <Board> m_boards;
    };

#endif