#include "Runner.h"

auto Runner::addPlayer ( u8 color, std::unique_ptr<Player>&& player ) -> void
    {
    assert ( color == BLACK || color == WHITE );
    player->color = color;
    m_players [color] = std::move ( player );
    }

auto Runner::createDefaultBoard () -> void 
    {
    Board& b = getBoard ();
    b.reset ();

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
            b.setPiece(CASTLE | color, 0 + (7 * columnMultiplier), row);
            }
        }
    }

auto Runner::doMove ( Board& b, Move& m ) -> void
    {
    const u8 player = m_whiteMove? WHITE: BLACK;
    const bool valid = b.doMove ( m );

    if ( !valid )
        throw std::runtime_error ("player attempted an illegal move");

    m_whiteMove = !m_whiteMove;
    m_state = b.getBoardState ( player );
    }

auto Runner::run () -> void 
    {
    Board& board = getBoard ();
    board.reset ();
    createDefaultBoard ();

    for (;;)
        {
        if ( tick() )
            return;
        }
    }