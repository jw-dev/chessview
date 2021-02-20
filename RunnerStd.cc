#include "Runner.h"

auto RunnerStd::getBoard () -> Board& 
    {
    return m_board;
    }

auto RunnerStd::tick () -> bool 
    {
    const auto& p = m_players [ m_whiteMove? WHITE: BLACK];
    Move move = p->getMove ( m_board );
    doMove ( m_board, move );

    return (m_state == STATE_NORMAL)? false: true;
    }