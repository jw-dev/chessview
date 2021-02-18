
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <functional>
#include "Board.h"

struct Color 
    {
    u8 red, green, blue, alpha;
    };

struct Mouse 
    {
    int x, y;
    bool isGrabbed; // true if we've held lmb
    bool isReleased; // true if we've just released lmb
    int grabx, graby; // grab position
    };

struct Viewer 
    {
    Viewer (const std::string& name, int width, int height); 
    ~Viewer ();
    Viewer (const Viewer& other) = delete;
    bool operator= (const Viewer& other) = delete;

    auto draw ( Board& board) -> void; 
    auto update () -> void; // updates current key presses 

    auto isPressed ( int key ) const -> bool;
    auto isQuit () const -> bool;

    std::function<void(Move&)> onNewMove;

protected: 
    SDL_Window *m_window = nullptr;
    SDL_Renderer *m_renderer = nullptr;
    std::unordered_map<u8, SDL_Texture*> m_pieceTextures = {};
    std::vector<int> m_keys;
    bool m_quit;
    int m_width;
    Mouse m_mouse;

    auto drawTiles ( const Board& board ) -> void;
    auto drawPieces ( const Board& board ) -> void;
    auto doMovement () -> void;

    auto updateMouse () -> void;
    auto initTextures () -> void;
    auto setColor ( Color c ) const -> void;
    };