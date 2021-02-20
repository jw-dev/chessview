#include "Runner.h"

#define WINDOW_TITLE "chessview"
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 480

RunnerUI::RunnerUI () 
  : Runner (),
    m_states {},
    m_viewer { "chessview", WINDOW_WIDTH, WINDOW_HEIGHT }
    {
    m_states.emplace_back ();
    m_viewer.onNewMove = [this] ( Move& move )
        {
        if ( m_players.size() == 2 && m_players[WHITE] != nullptr && m_players[BLACK] != nullptr )
            return; // if 2 computers, ignore 

        for ( u8 color: { WHITE, BLACK } )
            if ( m_players[color] == nullptr ) 
                move.color = color;

        // is it even our turn?
        if ( ( move.color == WHITE && !m_whiteMove ) || ( move.color == BLACK && m_whiteMove ) ) 
            return; // nope

        // check promotion 
        Board& b = getBoard ();
        const u8 piece = b.pieceAt ( move.fromCol, move.fromRow ) & Board::TYPE_MASK;

        if ( piece == PAWN && ((move.color == WHITE && move.toRow == 7) || (move.color == BLACK && move.toRow == 0)) )
            move.promotion = QUEEN;

        if ( b.isMoveLegal ( move ) ) 
            doMove ( b, move );
        };
    }

auto RunnerUI::getBoard () -> Board& 
    {
    return m_states [ m_index ];
    }

auto RunnerUI::tick () -> bool
    {
    m_viewer.update ();

    // handle quit
    if ( m_viewer.isQuit() )
        return true;

    // handle pause 
    if ( m_viewer.isPressed ( SDLK_SPACE ) ) 
        m_paused = !m_paused;

    // tick board state if unpaused, or if we're paused and moving forward in states
    if ( m_state == STATE_NORMAL && ( !m_paused || m_viewer.isPressed ( SDLK_RIGHT ) ) )
        {
        if ( m_index == m_states.size() - 1 )
            {
            Board& currentBoard = m_states.back ();
            Board newBoard ( currentBoard );

            const std::unique_ptr<Player>& p = m_players [ m_whiteMove ? WHITE: BLACK ];
            if (p)
                {
                std::cout << "doing move for player: " << m_whiteMove << std::endl;
                Move move = p->getMove ( newBoard );
                doMove ( newBoard, move );
                m_states.push_back ( newBoard );
                m_index++;
                }
            }
        else if ( m_index < m_states.size() - 1 )
            m_index++;
        
        }
    // handle undo if we are paused
    else if ( m_paused && m_viewer.isPressed ( SDLK_LEFT ) ) 
        undo ();
        
    // draw board
    m_viewer.draw ( getBoard() );

    // add delay if computer vs computer
    if ( !m_paused && m_players[WHITE] && m_players[BLACK] ) 
        SDL_Delay ( 250 );

    return false;
    } 

auto RunnerUI::undo () -> void
    {
    if ( m_index ) 
        {
        m_index--;
        m_whiteMove = !m_whiteMove;
        m_state = STATE_NORMAL;
        }
    }