#ifndef RUNNER_H
#define RUNNER_H

#include <unordered_map>
#include <sstream>
#include <stack>
#include <assert.h>

#include "Board.h"
#include "Viewer.h"
#include "Player.h"


struct Runner 
    {
    Runner ();
    virtual ~Runner() = default;

    auto addPlayer ( u8 color, std::unique_ptr<Player>&& player ) -> void;
    auto getState () -> BoardState;
    auto getWinner () -> u8;
    auto run () -> void;
protected: 
    std::map <u8, std::unique_ptr<Player>> m_players;
    BoardState m_state;
    u8 m_winner;

    auto createDefaultBoard () -> void;
    auto doMove ( Board& b, Move& m ) -> void;
    virtual auto getBoard () -> Board& = 0;
    virtual auto tick () -> bool = 0;
    };

struct RunnerUI final: public Runner 
    {
    RunnerUI ();
private:
    auto getBoard () -> Board& override;
    auto tick () -> bool override;
    auto undo () -> void;

    std::vector <Board> m_states;
    std::size_t m_index = 0U;
    Viewer m_viewer;
    bool m_paused = false;
    };

struct RunnerStd final: public Runner 
    {
    RunnerStd ();
private: 
    auto getBoard () -> Board& override;
    auto tick () -> bool override;
    Board m_board;
    };

#endif