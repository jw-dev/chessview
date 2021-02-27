#include "Viewer.h"

namespace 
    {
    const Color LIGHT_COLOR = { 117, 102, 99, 255 };
    const Color DARK_COLOR = { 142, 127, 114, 255 };
    const Color LAST_MOVE_COLOR = { 102, 102, 0, 125 };
    const Color ATTACKED_COLOR = { 255, 0, 0, 125 };
    const Color HOVER_COLOR = { 172, 157, 144, 255 };
    }

Viewer::Viewer (const std::string& name, int width, int height)
  : onNewMove (nullptr),
    m_keys (),
    m_quit (false),
    m_width (width),
    m_mouse ()
    {
    SDL_Init (SDL_INIT_VIDEO);
    IMG_Init (IMG_INIT_PNG);

    m_window = SDL_CreateWindow (
        name.c_str(), 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED,
        width, 
        height,
        SDL_WINDOW_OPENGL);

    m_renderer = SDL_CreateRenderer (
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED);
    
    SDL_SetRenderDrawBlendMode (m_renderer, SDL_BLENDMODE_BLEND);

    initTextures ();
    }

Viewer::~Viewer () 
    {
    SDL_DestroyWindow ( m_window );
    SDL_DestroyRenderer ( m_renderer );
    IMG_Quit ();
    SDL_Quit ();
    }

auto Viewer::initTextures () -> void 
    {
    // Initialise textures
    for (u8 color: { BLACK, WHITE })
        for (u8 piece: { BISHOP, KING, KNIGHT, PAWN, QUEEN, ROOK, CASTLE })
            {
            std::stringstream ss;
            ss << "Img/";
            switch (piece) 
                {
                case BISHOP: ss << "bishop"; break;
                case KING:   ss << "king"; break;
                case KNIGHT: ss << "knight"; break;
                case QUEEN:  ss << "queen"; break;
                case ROOK:   ss << "rook"; break;
                case PAWN:   ss << "pawn"; break;
                case CASTLE: ss << "rook"; break;
                }
            ss << "_"
            << (color == BLACK ? "black" : "white")
            << ".png";

            SDL_Surface *surface = IMG_Load (ss.str().c_str());
            if (surface)
                {
                SDL_Texture *txt = SDL_CreateTextureFromSurface ( m_renderer, surface );
                if (txt) 
                    m_pieceTextures [color | piece] = txt;
                SDL_FreeSurface ( surface );
                }
            } 
    }


auto Viewer::setColor ( Color c ) const -> void
    {
    SDL_SetRenderDrawColor ( m_renderer, c.red, c.green, c.blue, c.alpha );
    }



auto Viewer::update () -> void 
    {
    m_keys.clear ();

    SDL_Event e; 
    while ( SDL_PollEvent(&e) )
        {
        switch ( e.type )
            {
            case SDL_KEYDOWN:
                m_keys.push_back (e.key.keysym.sym);
                break;
            case SDL_QUIT:
                m_quit = true;
                break;
            default:
                break;
            }
        }
    updateMouse ();
    }

auto Viewer::isPressed ( int key ) const -> bool
    {
    return std::find ( m_keys.begin(), m_keys.end(), key ) != m_keys.end ();
    }

auto Viewer::isQuit () const -> bool
    {
    return m_quit;
    }

auto Viewer::updateMouse () -> void
    {
    auto state = SDL_GetMouseState ( &m_mouse.x, &m_mouse.y );
    m_mouse.isReleased = false;

    if ( state & SDL_BUTTON(SDL_BUTTON_LEFT) ) // pressing lmb
        {
        if ( !m_mouse.isGrabbed )
            {
            m_mouse.isGrabbed = true;
            m_mouse.grabx = m_mouse.x;
            m_mouse.graby = m_mouse.y;
            }
        }
    else // not pressing lmb
        {
        if ( m_mouse.isGrabbed ) 
            m_mouse.isReleased = true;
        m_mouse.isGrabbed = false;
        }
    }

auto Viewer::drawTiles ( const Board& board ) -> void
    {
    const u8 tileSize = m_width / Board::GRID_LENGTH;
    const Move& last = board.lastMove;
    const bool isMoveZero = !last.fromCol && !last.fromRow && !last.toCol && !last.toRow;

    for ( u8 x = 0; x < Board::GRID_LENGTH; ++x ) 
        for ( u8 y = 0; y < Board::GRID_LENGTH; ++y )
            {
            const u8 row = Board::GRID_LENGTH - 1 - y;
            const u8 piece = board.pieceAt ( x, y );
            const SDL_Rect tile { x * tileSize, row * tileSize, tileSize, tileSize };

            // check for hovering over a tile
            if ( m_mouse.x > tile.x && m_mouse.x <= tile.x + tileSize && m_mouse.y > tile.y && m_mouse.y <= tile.y + tileSize)
                {
                setColor ( HOVER_COLOR );
                }
            // if this tile participated in the last move, highlight it specially 
            else if  ( !isMoveZero && ( ( last.fromCol == x && last.fromRow == y ) || ( last.toCol == x && last.toRow == y ) ) )
                {
                setColor ( LAST_MOVE_COLOR );
                }
            // highlight tile in red, if it is a king in check
            else if ( ( piece & Board::TYPE_MASK ) == KING && board.isAttacked ( x, y ) ) 
                {
                setColor ( ATTACKED_COLOR );
                }
            // default, no special activity 
            else 
                {
                setColor ( (x + y) & 1? DARK_COLOR: LIGHT_COLOR );
                }
            SDL_RenderFillRect ( m_renderer, &tile );
            }
    }
    
auto Viewer::drawPieces ( const Board& board ) -> void
    {
    const u8 tileSize = m_width / Board::GRID_LENGTH;
    for ( u8 x = 0; x < Board::GRID_LENGTH; ++x ) 
        for ( u8 y = 0; y < Board::GRID_LENGTH; ++y )
            {
            const u8 row = Board::GRID_LENGTH - 1 - y;
            const u8 piece = board.pieceAt ( x, y );
            SDL_Rect tile { x * tileSize, row * tileSize, tileSize, tileSize };
            
            if ( piece != EMPTY ) 
                {
                SDL_Texture* texture = m_pieceTextures [ piece ];
                // check for if we're grabbing this piece 
                if ( m_mouse.isGrabbed ) 
                    {
                    const u8 gridGrabX = m_mouse.grabx / tileSize;
                    const u8 gridGrabY = Board::GRID_LENGTH - 1 - (m_mouse.graby / tileSize);
                    if ( x == gridGrabX && y == gridGrabY )
                        {
                        // move draw position for texture 
                        tile.x = m_mouse.x - (m_mouse.grabx - tile.x) ;
                        tile.y = m_mouse.y - (m_mouse.graby - tile.y) ;
                        }
                    }
                SDL_RenderCopy ( m_renderer, texture, NULL, &tile );
                }
            }
    }

auto Viewer::doMovement () -> void
    {
    if ( !onNewMove )
        return;

    const u8 tileSize = m_width / Board::GRID_LENGTH;
    const u8 fromCol = m_mouse.grabx / tileSize;
    const u8 toCol = m_mouse.x / tileSize;
    const u8 fromRow = Board::GRID_LENGTH - 1 - (m_mouse.graby / tileSize);
    const u8 toRow = Board::GRID_LENGTH - 1 - (m_mouse.y / tileSize);
    
    Move m { fromCol, toCol, fromRow, toRow };
    onNewMove ( m );
    }

auto Viewer::draw (Board& board) -> void
    {
    SDL_SetRenderDrawColor (m_renderer, 255, 255, 255, 255);
    SDL_RenderClear (m_renderer);
    drawTiles ( board );
    drawPieces ( board );
    if ( m_mouse.isReleased ) 
        doMovement ();
    SDL_RenderPresent (m_renderer);
    }