#include "Runner.h"

RunnerUI::RunnerUI (const std::shared_ptr<Player>& white, const std::shared_ptr<Player>& black)
  : m_boards {},
    m_viewer { "chessview", 400, 400 }
    {
    for (const u8 color: {WHITE, BLACK}) 
        {
        m_players [color] = (color == WHITE) ? white: black;
        m_players [color]->setColor (color);
        }
    (void) m_boards.create();
    createDefaultBoard();
    }
    
RunnerUI::~RunnerUI() 
    {
    
    }

auto RunnerUI::board() -> Board& 
    {
    return m_boards.get();
    }

auto RunnerUI::doNewMove() -> void 
    {
    if (m_boards.atEnd()) 
        {
        Board& newBoard = m_boards.create();
        const std::shared_ptr<Player>& p = m_players [ m_whiteMove ? WHITE: BLACK ];
        if (p)
            {
            Move move = p->getMove ( newBoard );
            newBoard.doMove ( move );
            }
        }
    else 
        {
        m_boards.move(1);
        }
    }

auto RunnerUI::undo() -> void 
    {
     if (!m_boards.atBeg())
        {
        m_boards.move(-1);
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