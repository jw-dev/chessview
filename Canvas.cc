#include "Canvas.h"

Canvas::Canvas ( const std::string& name, int width, int height )
  : width { width },
    m_window {nullptr},
    m_renderer {nullptr},
    m_textures {}
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
    }

auto Canvas::addTexture (u8 key, const std::string& path) -> void
    {
    const char *p = path.c_str();
    SDL_Surface *surface = IMG_Load (p);
    if (surface)
        {
        SDL_Texture *txt = SDL_CreateTextureFromSurface ( m_renderer, surface );
        SDL_FreeSurface ( surface );
        if (txt) 
            m_textures [key] = txt;
        }
    }

auto Canvas::drawTexture (u8 key, int x, int y, int w, int h) const -> void
    {
    SDL_Rect rect 
        {
        x, y, w, h
        };
    if ( m_textures.find(key) != m_textures.end() )
        {
        SDL_Texture * texture = m_textures.at (key);
        SDL_RenderCopy (m_renderer, texture, NULL, &rect);
        }
    }

auto Canvas::square (int x, int y, int w, int h) const -> void
    {
    SDL_Rect rect 
        {
        x, y, w, h,
        };
    SDL_RenderFillRect ( m_renderer, &rect );
    }

auto Canvas::brush (u8 r, u8 g, u8 b, u8 alpha) const -> void
    {
    SDL_SetRenderDrawColor ( m_renderer, r, g, b, alpha );
    }


auto Canvas::setTitle (const std::string& title) const -> void
    {
    SDL_SetWindowTitle ( m_window, title.c_str() );
    }

auto Canvas::setSize ( int w, int h ) const -> void
    {
    SDL_SetWindowSize ( m_window, w, h );
    }

auto Canvas::tick(int delay) const -> void
    {
    //  // Clear window 
    // SDL_SetRenderDrawColor (m_renderer, 255, 255, 255, 255);
    // SDL_RenderClear (m_renderer);

    SDL_RenderPresent (m_renderer);
    SDL_Delay (delay);
    }