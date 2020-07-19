#include "Runner.h"

RunnerStd::RunnerStd (const std::shared_ptr<Player>& white, const std::shared_ptr<Player>& black)
  : m_board { }
    {
    assert ( white );
    assert ( black );
    for (const u8 color: {WHITE, BLACK}) 
        {
        m_players [color] = (color == WHITE) ? white: black;
        m_players [color]->setColor (color);
        }
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