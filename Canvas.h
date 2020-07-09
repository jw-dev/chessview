#ifndef CANVAS_H
#define CANVAS_H

#include <string>
#include <unordered_map>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using u8 = std::uint_least8_t;

struct Canvas 
    {
    int width;

    Canvas ( const std::string& name, int width, int height );
    ~Canvas ();
    Canvas ( const Canvas& other ) = delete;
    bool operator= ( const Canvas& other ) = delete;

    auto addTexture (u8 key, const std::string& path) -> void;
    auto drawTexture (u8 key, int x, int y, int w, int h) const -> void; 
    auto brush (u8 r, u8 g, u8 b, u8 alpha) const -> void;
    auto square (int x, int y, int w, int h) const -> void; 
    auto tick(int delay) const -> void;

    auto setTitle (const std::string& title) const -> void;
    auto setSize ( int w, int h ) const -> void;
protected:
    SDL_Window *m_window; 
    SDL_Renderer *m_renderer;
    std::unordered_map <u8, SDL_Texture*> m_textures; 
    };

#endif