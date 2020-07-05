#ifndef DRAW_H 
#define DRAW_H 

#include <map>
#include <string>
#include <sstream>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include "Board.h"

struct Draw 
    {

    Draw (const Board& board, const char * title, unsigned width, unsigned height); 
    Draw (const Draw& other) = delete;
    Draw& operator=(const Draw& other) = delete;
    auto draw () -> void;
private: 
    unsigned m_tileSize;
    SDL_Window *m_window; 
    SDL_Renderer *m_renderer;
    const Board& m_board; 
    std::map <u8, std::map<u8, SDL_Texture*>> m_pieceTextures;
    };

#endif