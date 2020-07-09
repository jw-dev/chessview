#include "Runner.h"

Runner::Runner ( Viewer* viewer )
  : gameState {STATE_NORMAL},
    winner {0},
    m_viewer {viewer},
    m_players {},
    m_board {},
    m_whiteMove {true}
    {
    // Pawns 
    for (int row: {1, 6}) 
        for (int column = 0; column < 8; ++column)
            {
            u8 color = row == 1 ? WHITE: BLACK; 
            m_board.setPiece (PAWN | color, column, row);
            }

    for (int row: {0, 7})
        {
        u8 color = row == 0 ? WHITE: BLACK;
        m_board.setPiece(QUEEN | color, 3, row);
        m_board.setPiece(KING | color, 4, row);
        for (int columnMultiplier: {0, 1})
            {
            m_board.setPiece(BISHOP | color, 2 + (3 * columnMultiplier), row);
            m_board.setPiece(KNIGHT | color, 1 + (5 * columnMultiplier), row);
            m_board.setPiece(ROOK | color, 0 + (7 * columnMultiplier), row);
            }
        }
    }

Runner::~Runner ()
    {
    for (auto player: m_players)
        {
        if (player.second)
            {
            delete player.second;
            player.second = nullptr;
            }
        }
    }

auto Runner::addPlayer (u8 color, Player* p) -> void
    {
    if (p)
        {
        color = color & Board::COLOR_MASK;
        p->setColor ( color );
        m_players [color] = p;
        }
    }

auto Runner::tick () -> bool
    {
    if (gameState == STATE_NORMAL)
        {
        u8 player = m_whiteMove ? WHITE: BLACK;
        if (m_board.isCheckmate (player))
            {
            gameState = STATE_CHECKMATE;
            winner = m_whiteMove ? BLACK: WHITE; // Reversed
            }
        else if (m_board.isStalemate (player))
            {
            gameState = STATE_STALEMATE_NO_MOVES; // TODO
            }
        else 
            {
            Player* p = m_players [player];
            if (p)
                {
                Move move = p->getMove ( m_board );
                bool isMoveLegal = m_board.doMove ( move );
                if (!isMoveLegal) 
                    throw std::runtime_error ("player attempted an illegal move");
                }
            }
        m_whiteMove = !m_whiteMove;
        }
    if (m_viewer) 
        {
        m_viewer->draw(m_board);
        }
    return gameState == STATE_NORMAL;
    }