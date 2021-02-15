#include "Runner.h"

RunnerUI::RunnerUI (const std::shared_ptr<Player>& white, const std::shared_ptr<Player>& black)
  : m_viewer { "chessview", 480, 480 },
    Runner { white, black }
    {
    m_states.emplace_back (); // create empty board
    createDefaultBoard(); // populate board
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
        if (p)
            {
            Move move = p->getMove ( newBoard );
            newBoard.doMove ( move );
            }
        m_states.push_back ( newBoard );
        }
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
    bool paused = false;
    for (;;)
        {
        m_viewer.update ();

        // handle quit
        if ( m_viewer.isQuit() )
            return;

        // handle pause 
        if ( m_viewer.isPressed ( SDLK_SPACE ) ) 
            paused = !paused;

        // tick board state if unpaused, or if we're paused and moving forward in states
        if ( gameState == STATE_NORMAL && ( !paused || m_viewer.isPressed ( SDLK_RIGHT ) ) )
            tick ();
        // handle undo if we are paused
        else if ( paused && m_viewer.isPressed ( SDLK_LEFT ) ) 
            undo ();
            
        // draw board
        m_viewer.draw ( board() );

        if (!paused) 
            SDL_Delay ( 100 );
        }
    }