#include "Viewer.h"


Viewer::Viewer (const std::string& name, int width, int height)
  : m_width (width)
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

    // Initialise textures
    for (u8 color: { BLACK, WHITE })
        for (u8 piece: { BISHOP, KING, KNIGHT, PAWN, QUEEN, ROOK })
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

Viewer::~Viewer () 
    {
    SDL_DestroyWindow ( m_window );
    SDL_DestroyRenderer ( m_renderer );
    IMG_Quit ();
    SDL_Quit ();
    }

auto Viewer::setColor (float r, float g, float b, float alpha) -> void
    {
    SDL_SetRenderDrawColor ( m_renderer, getFloatColor(r), getFloatColor(g), getFloatColor(b), getFloatColor(alpha) );
    }

auto Viewer::getFloatColor (float c) -> int
    {
    return static_cast<int>(std::floor(255.0f * c));
    }

auto Viewer::draw (const Board& board) -> void
    {
    SDL_SetRenderDrawColor (m_renderer, 255, 255, 255, 255);
    SDL_RenderClear (m_renderer);
 
    const u8 tileSize = m_width / Board::GRID_LENGTH;

    static const auto getActualRow =
        [] (u8 y)
            {
            return Board::GRID_LENGTH - 1 - y;
            };

    static const auto drawTile = 
        [&board, &tileSize, this] (u8 x, u8 y) -> void
            {
            const u8 actualRow = getActualRow (y);
            const u8 piece = board.pieceAt ( x, y );

            // Rectangle to draw in 
            const SDL_Rect tile { x * tileSize, actualRow * tileSize, tileSize, tileSize };

            // Draw actual tile 
            const float colorAlt = (x + y) & 1? 0.1f: 0.0f;
            setColor ( 0.46f + colorAlt, 0.40f + colorAlt, 0.39f + colorAlt, 1.0f );

            SDL_RenderFillRect ( m_renderer, &tile );

            // Highlight tile, if it participated in the last move
            const Move last = board.lastMove;
            if ( (last.fromCol == x && last.fromRow == y) || (last.toCol == x && last.toRow == y) )
                {
                setColor ( 0.4f, 0.4f, 0.0f, 0.25f );
                SDL_RenderFillRect ( m_renderer, &tile );
                }
            // Highlight tile (in red), if it is attacked and is a King (to signify check).
            else if ( ((piece & Board::TYPE_MASK) == KING) && board.isAttacked (x, y) )
                {
                setColor ( 1.0f, 0.0f, 0.0f, 0.25f );
                SDL_RenderFillRect ( m_renderer, &tile );
                }

            // Draw texture 
            if ( piece && m_pieceTextures.find(piece) != m_pieceTextures.end() )
                {
                SDL_Texture* texture = m_pieceTextures [piece];
                SDL_RenderCopy ( m_renderer, texture, NULL, &tile );
                }
            };

    for (u8 x = 0; x < Board::GRID_LENGTH; ++x)
        for (u8 y = 0; y < Board::GRID_LENGTH; ++y)
            {
            drawTile (x, y);
            }

    SDL_RenderPresent (m_renderer);
    SDL_Delay (400);
    }