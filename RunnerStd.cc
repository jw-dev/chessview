#include "Runner.h"

RunnerStd::RunnerStd(std::unique_ptr<Player> &&white,
                     std::unique_ptr<Player> &&black)
    : m_board{} {
    addPlayer(WHITE, std::move(white));
    addPlayer(BLACK, std::move(black));
}

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