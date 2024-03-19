#include "Runner.h"

RunnerStd::RunnerStd() : m_board() {}

auto RunnerStd::getBoard() -> Board & { return m_board; }

auto RunnerStd::tick() -> bool {
    if (m_state != STATE_NORMAL)
        return true;

    const u8 player = m_board.whiteMove() ? WHITE : BLACK;
    const auto &p = m_players[player];
    Move move = p->getMove(m_board);
    doMove(m_board, move);
    return false;
}