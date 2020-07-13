#include "Runner.h"

Runner::Runner (Player* white, Player* black)
  : gameState {STATE_NORMAL},
    winner {0},
    m_players {},
    m_whiteMove {true}
    {
    for (const u8 color: {WHITE, BLACK}) 
        {
        m_players [color] = color == WHITE? white: black; 
        m_players [color]->setColor (color);
        }
    }

Runner::~Runner() 
    {
    for (const auto& k: m_players) 
        if (k.second) 
            delete k.second;
    }

auto Runner::createDefaultBoard () -> void
    {
    Board& b = board();
    // Pawns 
    for (int row: {1, 6}) 
        for (int column = 0; column < 8; ++column)
            {
            u8 color = row == 1 ? WHITE: BLACK; 
            b.setPiece (PAWN | color, column, row);
            }

    for (int row: {0, 7})
        {
        u8 color = row == 0 ? WHITE: BLACK;
        b.setPiece(QUEEN | color, 3, row);
        b.setPiece(KING | color, 4, row);
        for (int columnMultiplier: {0, 1})
            {
            b.setPiece(BISHOP | color, 2 + (3 * columnMultiplier), row);
            b.setPiece(KNIGHT | color, 1 + (5 * columnMultiplier), row);
            b.setPiece(ROOK | color, 0 + (7 * columnMultiplier), row);
            }
        }
    }

auto Runner::tick() -> bool 
    {
    if (gameState == STATE_NORMAL)
        {
        const u8 player = m_whiteMove ? WHITE: BLACK;
        Board& b = board();

        if (b.isCheckmate (player)) 
            {
            gameState = STATE_CHECKMATE;
            winner = m_whiteMove ? BLACK: WHITE; // Reversed
            }
        else if (b.isStalemate (player)) 
            {
            gameState = STATE_STALEMATE_NO_MOVES; // TODO
            }
        else 
            {
            doNewMove();
            }
        m_whiteMove = !m_whiteMove;
        }
    return gameState == STATE_NORMAL;
    }