#ifndef RUNNER_H
#define RUNNER_H

#include <assert.h>
#include <sstream>
#include <stack>
#include <unordered_map>

#include "Board.h"
#include "FEN.h"
#include "Player.h"
#include "Viewer.h"

struct Runner {
    Runner();
    virtual ~Runner() = default;

    auto addPlayer(u8 color, std::unique_ptr<Player> &&player) -> void;
    auto getState() -> BoardState;
    auto getWinner() -> u8;
    auto run() -> std::string;

  protected:
    std::map<u8, std::unique_ptr<Player>> m_players;
    BoardState m_state;
    u8 m_winner;
    Move m_lastMove;
    u16 m_staleMoveHalfClock = 0U;
    u16 m_fullMoves = 0U;

    auto createDefaultBoard() -> void;
    auto doMove(Board &b, Move &m) -> void;
    virtual auto getBoard() -> Board & = 0;
    virtual auto tick() -> bool = 0;
};

struct RunnerUI final : public Runner {
    RunnerUI();

  private:
    auto getBoard() -> Board & override;
    auto tick() -> bool override;
    auto undo() -> void;

    std::vector<Board> m_states;
    std::size_t m_index = 0U;
    Viewer m_viewer;
    bool m_paused = false;
};

struct RunnerStd final : public Runner {
    RunnerStd();

  private:
    auto getBoard() -> Board & override;
    auto tick() -> bool override;
    Board m_board;
};

#endif