#include "Runner.h"

Runner::Runner()
  : m_players (), 
    m_state (STATE_NORMAL), 
    m_winner ( 0U ) 
    {
    }

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
    if ( !b.doMove ( m ) )
        throw std::runtime_error ("player attempted an illegal move");
    m_state = b.getBoardState ();
    }

auto Runner::run () -> std::string 
    {
    Board& board = getBoard ();
    board.reset ();
    createDefaultBoard ();

    for (;;)
        {
        if ( tick() )
            break;
        }
    // after we're done, return a FEN of the final board state.
    Board& final = getBoard ();
    BoardState state = final.getBoardState ();
    std::ostringstream oss;
    switch ( state ) 
        {
        case STATE_CHECKMATE: 
            {
            const u8 winner = final.whiteMove()? BLACK: WHITE;
            oss << "Checkmate ("
                << ( winner == WHITE? "White": "Black" )
                << " wins)";
            break;
            }
        case STATE_STALEMATE: oss << "Stalemate"; break;
        case STATE_FORCED_DRAW_FIFTY_MOVES: oss << "Draw by 50 move rule"; break;
        case STATE_FORCED_DRAW_INSUFFICIENT_MATERIAL: oss << "Draw by insufficient material"; break;
        }
    oss << "\n"
        << FEN::toFEN ( &final );
    return oss.str ();
    }