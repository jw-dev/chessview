#include "Runner.h"

RunnerUI::RunnerUI (const std::shared_ptr<Player>& white, const std::shared_ptr<Player>& black)
  : Runner { white, black },
    m_states {},
    m_viewer { "chessview", 480, 480 }
    {
    m_states.emplace_back (); // create empty board
    createDefaultBoard(); // populate board
    }

RunnerUI::RunnerUI (const std::shared_ptr<Player>& black)
  : Runner { black },
    m_states {},
    m_viewer { "chessview", 480, 480 }
    {
    m_states.emplace_back (); // create empty board
    createDefaultBoard(); // populate board

    m_viewer.onNewMove = [this] (Move& move) 
        {
        Board& currentBoard = m_states.back ();

        // set color of move 
        move.color = m_whiteMove ? WHITE: BLACK;

        // check promotion 
        const u8 piece = currentBoard.pieceAt ( move.fromCol, move.fromRow ) & Board::TYPE_MASK;

        if ( piece == PAWN && ((move.color == WHITE && move.toRow == 7) || (move.color == BLACK && move.toRow == 0)) )
            move.promotion = QUEEN;

        if ( currentBoard.isMoveLegal ( move ) )
            {
            Board newBoard ( currentBoard );
            newBoard.doMove ( move );
            m_states.push_back ( newBoard );
            m_whiteMove = !m_whiteMove;
            m_index++;
            }
        };
    }
    
RunnerUI::~RunnerUI() 
    {
    
    }

auto RunnerUI::board() -> Board& 
    {
    return m_states[m_index];
    }

auto RunnerUI::doNewMove() -> void 
    {
    if ( m_index == m_states.size() - 1 )
        {
        Board& currentBoard = m_states.back ();
        Board newBoard ( currentBoard );

        const std::shared_ptr<Player>& p = m_players [ m_whiteMove ? WHITE: BLACK ];
        if (!p)
            return;

        Move move = p->getMove ( newBoard );
        newBoard.doMove ( move );
        m_states.push_back ( newBoard );
        }
    m_whiteMove = !m_whiteMove;
    m_index++;
    }

auto RunnerUI::undo() -> void 
    {
    if ( m_index ) 
        {
        m_index--;
        m_whiteMove = !m_whiteMove;
        gameState = STATE_NORMAL;
        }
    }

auto RunnerUI::run() -> void 
    {
    for (;;)
        {
        m_viewer.update ();

        // handle quit
        if ( m_viewer.isQuit() )
            return;

        // handle pause 
        if ( m_viewer.isPressed ( SDLK_SPACE ) ) 
            m_paused = !m_paused;

        // tick board state if unpaused, or if we're paused and moving forward in states
        if ( gameState == STATE_NORMAL && ( !m_paused || m_viewer.isPressed ( SDLK_RIGHT ) ) )
            tick ();
        // handle undo if we are paused
        else if ( m_paused && m_viewer.isPressed ( SDLK_LEFT ) ) 
            undo ();
            
        // draw board
        m_viewer.draw ( board() );

        // add delay if computer vs computer
        if ( !m_paused && m_players[WHITE] && m_players[BLACK] ) 
            SDL_Delay ( 250 );
        }
    }