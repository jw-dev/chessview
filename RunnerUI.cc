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
        SDL_Event e; 
        while ( SDL_PollEvent(&e) ) 
            {
            switch (e.type)
                {
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) 
                        {
                        case SDLK_LEFT: 
                            if (paused) 
                                undo();
                            break;
                        case SDLK_RIGHT:
                            if (paused && gameState == STATE_NORMAL)
                                tick();
                            break;
                        case SDLK_SPACE:
                            paused = !paused;
                            break;
                        }
                    break;
                case SDL_QUIT:
                    return;
                }
            }
        if ( !paused && gameState == STATE_NORMAL )
            {
            tick();
            }
        m_viewer.draw ( board() );
        if (!paused) 
            SDL_Delay ( 300 );
        }
    }