#ifndef RUNNER_H
#define RUNNER_H

#include <unordered_map>
#include <sstream>
#include <stack>
#include <assert.h>

#include "Board.h"
#include "Viewer.h"
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
    GameState gameState = STATE_NORMAL;
    u8 winner = 0U;

    Runner (const std::shared_ptr<Player>& black);
    Runner (const std::shared_ptr<Player>& white, const std::shared_ptr<Player>& black);
    virtual ~Runner();

    virtual auto run() -> void = 0;
    virtual auto reset() -> void;
protected:
    std::unordered_map <u8, std::shared_ptr<Player>> m_players;
    bool m_whiteMove;

    virtual auto board () -> Board& = 0;
    virtual auto doNewMove() -> void = 0;
    virtual auto tick() -> bool;
    virtual auto createDefaultBoard() -> void;
    };


struct RunnerUI final: public Runner 
    {
    RunnerUI (const std::shared_ptr<Player>& black);
    RunnerUI (const std::shared_ptr<Player>& white, const std::shared_ptr<Player>& black);
    ~RunnerUI() override; 

    auto run() -> void override;
private:
    std::vector <Board> m_states; 
    std::size_t m_index = 0U;

    Viewer m_viewer;
    bool m_paused = false;

    auto board() -> Board& override;
    auto doNewMove() -> void override;
    auto undo() -> void;
    };


struct RunnerStd final: public Runner 
    {
    RunnerStd (const std::shared_ptr<Player>& white, const std::shared_ptr<Player>& black);
    ~RunnerStd() override;

    auto run() -> void override;
private:
    Board m_board;

    auto board() -> Board& override;
    auto doNewMove() -> void override;
    };

#endif