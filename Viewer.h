
#include <sstream>
#include "Canvas.h"
#include "Board.h"

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

    auto setColor (float r, float g, float b, float alpha) -> void;
    static auto getFloatColor (float c) -> int;
    };



//         Canvas ( const std::string& name, int width, int height );
//     ~Canvas ();
//     Canvas ( const Canvas& other ) = delete;
//     bool operator= ( const Canvas& other ) = delete;

//     auto addTexture (u8 key, const std::string& path) -> void;
//     auto drawTexture (u8 key, int x, int y, int w, int h) const -> void; 
//     auto brush (u8 r, u8 g, u8 b, u8 alpha) const -> void;
//     auto square (int x, int y, int w, int h) const -> void; 
//     auto tick(int delay) const -> void;

//     auto setTitle (const std::string& title) const -> void;
//     auto setSize ( int w, int h ) const -> void;
// protected:
//     SDL_Window *m_window; 
//     SDL_Renderer *m_renderer;
//     std::unordered_map <u8, SDL_Texture*> m_textures; 