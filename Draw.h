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
    // Draw (Game *game, const char *title, unsigned width, unsigned height);
    auto draw () -> bool;
    // auto handleEvent(SDL_Event *e) -> void;
private: 
    unsigned m_width, m_height, m_tileSize;
    SDL_Window *m_window; 
    SDL_Renderer *m_renderer;
    const Board& m_board; 
    std::map <u8, std::map<u8, SDL_Texture*>> m_pieceTextures;
    
    // auto getPieceAtPosition (u8 x, u8 y) -> const Piece* ;
    // auto handleClick () -> void;
    // auto getTileColor (u8 x, u8 y, const Piece * piece) const -> SDL_Color; 
    // auto getTileRect (u8 x, u8 y) const -> SDL_Rect;

    
    
    // SDL_Point m_mousePoint;
    // bool m_isClicking;

    // const Piece * m_selectedPiece = nullptr;
    // u8 m_selectedColumn = -1, m_selectedRow = -1;
    };

#endif