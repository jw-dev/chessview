#include "Chess.h"


Chess::Chess (Player *white, Player *black)
    {
    white->setColor (WHITE);
    m_players [WHITE] = white;

    black->setColor (BLACK); 
    m_players [BLACK] = black;

    m_whiteMove = true;
    createDefaultBoard ();
    }

auto Chess::tick() -> Result 
    {
    if (endResult == None)
        {
        u8 player = m_whiteMove ? WHITE: BLACK; 
        if (m_board.isCheckmate (player)) 
            {
            endResult = Checkmate;
            winner = m_whiteMove ? BLACK: WHITE; // Reversed; 
            }
        else if (m_board.isStalemate (player)) 
            {
            endResult = Stalemate;
            }
        else 
            {
            Player* p = m_whiteMove ? m_players[WHITE]: m_players[BLACK]; 
            Move move = p->getMove (m_board);
            m_board.doMove (move);
            m_whiteMove = !m_whiteMove;
            }
        }
    return endResult;
    }

auto Chess::createDefaultBoard () -> void 
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