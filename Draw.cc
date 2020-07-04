#include "Draw.h"


Draw::Draw (const Board& board, const char * title, unsigned width, unsigned height)
: m_board (board),
  m_width (width),
  m_height (height),
  m_tileSize (width / board.GRID_LENGTH)
    {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    m_window = SDL_CreateWindow (
        title, 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED,
        width, 
        height,
        SDL_WINDOW_OPENGL);

    m_renderer = SDL_CreateRenderer (
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED);
        
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

            SDL_Surface * surface = IMG_Load (ss.str().c_str());
            SDL_Texture * text = SDL_CreateTextureFromSurface (m_renderer, surface);
            SDL_FreeSurface (surface);
            if (text) 
                {
                m_pieceTextures[piece][color] = text;
                }
            }
    }

auto Draw::draw () -> bool 
    {
    // Clear window 
    SDL_SetRenderDrawColor (m_renderer, 255, 255, 255, 255);
    SDL_RenderClear (m_renderer);

    const auto getTileRect =
        [&] (u8 x, u8 y) 
            {
            int drawY = Board::GRID_LENGTH - 1 - y;
            return SDL_Rect { x * m_tileSize, drawY * m_tileSize, m_tileSize, m_tileSize };
            };

    const auto getTileColor = 
        [&] (u8 x, u8 y)
            {
            float colorMult = 1.0f;
            const int index = x + y;
            if (index & 1) 
                colorMult += 0.2;
            return SDL_Color { 0x77 * colorMult, 0x66 * colorMult, 0x70 * colorMult, 255 }; 
            };

    // Get all actions 
    SDL_Event ev; 
    while (SDL_PollEvent(&ev)) 
        {
        switch (ev.type) 
            {
            case SDL_QUIT:
                return true;
            }
        }

    for (u8 x = 0; x < Board::GRID_LENGTH; ++x)
        for (u8 y = 0; y < Board::GRID_LENGTH; ++y)
            {
            // Draw initial board 

            const u8 piece = m_board.pieceAt (x, y); 
            const auto rect = getTileRect (x, y);
            SDL_Color color = getTileColor (x, y);

            SDL_SetRenderDrawColor (m_renderer, color.r, color.g, color.b, 255);
            SDL_RenderFillRect (m_renderer, &rect);

            if (piece) 
                {
                u8 type = piece & Board::TYPE_MASK;
                u8 color = piece & Board::COLOR_MASK;
                SDL_Texture * texture = m_pieceTextures[type][color];
                SDL_RenderCopy (m_renderer, texture, NULL, &rect);
                }
            }
    SDL_RenderPresent (m_renderer);
    SDL_Delay (50);
    return false;
    }
