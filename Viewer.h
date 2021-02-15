
#include <sstream>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Board.h"

struct Color 
    {
    u8 red, green, blue, alpha;
    };

struct Viewer 
    {
    Viewer (const std::string& name, int width, int height); 
    ~Viewer ();
    Viewer (const Viewer& other) = delete;
    bool operator= (const Viewer& other) = delete;

    auto draw (const Board& board) -> void; 
protected: 
    SDL_Window *m_window = nullptr;
    SDL_Renderer *m_renderer = nullptr;
    std::unordered_map<u8, SDL_Texture*> m_pieceTextures = {};
    int m_width;

    auto initTextures () -> void;
    auto setColor ( Color c ) const -> void;
    };