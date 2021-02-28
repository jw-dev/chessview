#include "Runner.h"
#include "Config.h"

RunnerUI::RunnerUI () 
  : Runner (),
    m_states {},
    m_viewer { Config::WindowTitle, Config::WindowWidth, Config::WindowHeight }
    {
    m_states.emplace_back ();
    m_viewer.onNewMove = [this] ( Move& move )
        {
        if ( m_players.size() == 2 && m_players[WHITE] != nullptr && m_players[BLACK] != nullptr )
            return; // if 2 computers, ignore 

        Board& b = getBoard ();
        const u8 playerColor = m_players[WHITE] == nullptr? WHITE: BLACK;
        bool whiteMove = b.whiteMove();

        // is it even our turn?
        if ( ( playerColor == WHITE && !whiteMove ) || ( playerColor == BLACK && whiteMove ) ) 
            return; // nope

        // check promotion 
        const u8 piece = b.pieceAt ( move.fromCol, move.fromRow ) & TYPE_MASK;

        if ( piece == PAWN && ((whiteMove && move.toRow == 7) || (!whiteMove && move.toRow == 0)) )
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
            const u8 player = currentBoard.whiteMove()? WHITE: BLACK;

            const std::unique_ptr<Player>& p = m_players [ player ];
            if (p)
                {
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
    m_viewer.draw ( getBoard(), m_lastMove );

    // add delay if computer vs computer
    if ( !m_paused && m_players[WHITE] && m_players[BLACK] ) 
        SDL_Delay ( Config::AfterMoveDelay );

    return false;
    } 

auto RunnerUI::undo () -> void
    {
    if ( m_index ) 
        {
        m_index--;
        m_state = STATE_NORMAL;
        }
    }