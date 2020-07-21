#include "Runner.h"

RunnerStd::RunnerStd (const std::shared_ptr<Player>& white, const std::shared_ptr<Player>& black)
  : Runner { white, black },
    m_board {} 
    {
    createDefaultBoard();
    }

RunnerStd::~RunnerStd() 
    {
        
    }

auto RunnerStd::run() -> void 
    {
    while ( gameState == STATE_NORMAL )
        tick ();
    }

auto RunnerStd::board() -> Board&
    {
    return m_board;
    }

auto RunnerStd::doNewMove() -> void
    {
    std::shared_ptr<Player>& p = m_players [ m_whiteMove ? WHITE: BLACK ];
    const Move m = p->getMove (m_board);
    bool valid = m_board.doMove ( m );
    if (!valid)
        throw std::runtime_error ("player attempted an illegal move");
    }