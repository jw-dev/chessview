#ifndef RUNNER_H
#define RUNNER_H

#include <unordered_map>
#include <sstream>

#include "Board.h"
#include "Canvas.h"
#include "Player.h"

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

    Runner (Canvas * canvas);
    Runner (const Runner& other) = delete;
    bool operator=(const Runner& other) = delete;
    ~Runner ();

    auto addPlayer (u8 color, Player* p) -> void;
    auto tick () -> bool;

protected:
    Canvas * m_canvas;
    std::unordered_map <u8, Player*> m_players;
    Board m_board;
    bool m_whiteMove;
    int m_tileSize;
    int m_delay;

    auto draw () -> void;
    };

#endif