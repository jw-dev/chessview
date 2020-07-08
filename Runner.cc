#include "Runner.h"

Runner::Runner ( Canvas* canvas )
  : gameState {STATE_NORMAL},
    winner {0},
    m_canvas {canvas},
    m_players {},
    m_board {},
    m_whiteMove {true},
    m_tileSize {canvas->width / Board::GRID_LENGTH},
    m_delay { 150 }
    {
    // Pawns 
    for (int row: {1, 6}) 
        for (int column = 0; column < 8; ++column)
            {
            u8 color = row == 1 ? WHITE: BLACK; 
            m_board.setPiece (PAWN | color, column, row);
            }

    for (int row: {0, 7})
        {
        u8 color = row == 0 ? WHITE: BLACK;
        m_board.setPiece(QUEEN | color, 3, row);
        m_board.setPiece(KING | color, 4, row);
        for (int columnMultiplier: {0, 1})
            {
            m_board.setPiece(BISHOP | color, 2 + (3 * columnMultiplier), row);
            m_board.setPiece(KNIGHT | color, 1 + (5 * columnMultiplier), row);
            m_board.setPiece(ROOK | color, 0 + (7 * columnMultiplier), row);
            }
        }

    // Add textures to viewer 
    if (m_canvas)
        {
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

                m_canvas->addTexture ( color | piece, ss.str().c_str() );
                }    
        }   
    }

Runner::~Runner ()
    {
    for (auto player: m_players)
        {
        if (player.second)
            {
            delete player.second;
            player.second = nullptr;
            }
        }
    if (m_canvas) delete m_canvas;
    }

auto Runner::addPlayer (u8 color, Player* p) -> void
    {
    if (p)
        {
        color = color & Board::COLOR_MASK;
        p->setColor ( color );
        m_players [color] = p;
        }
    }

auto Runner::draw () -> void
    {
    const auto setColor = 
        [this] (u8 x, u8 y) -> void
            {
            float colorMult = 1.0f;
            const int index = x + y;
            if (index & 1) 
                colorMult += 0.2;
            m_canvas->brush ( static_cast<u8>(0x77 * colorMult), static_cast<u8>(0x66 * colorMult), static_cast<u8>(0x70 * colorMult), 255 );
            };

    if (m_canvas) 
        {
        for (u8 x = 0; x < Board::GRID_LENGTH; ++x)
            for (u8 y = 0; y < Board::GRID_LENGTH; ++y)
                {
                const u8 drawY = Board::GRID_LENGTH - 1 - y;
                const u8 piece = m_board.pieceAt (x, y); 

                setColor(x, y);
                m_canvas->square ( x * m_tileSize, drawY * m_tileSize, m_tileSize, m_tileSize );
                if ( piece ) 
                    m_canvas->drawTexture ( piece, x * m_tileSize, drawY * m_tileSize, m_tileSize, m_tileSize );
                }
        m_canvas->tick (m_delay);
        }
    }

auto Runner::tick () -> bool
    {
    if (gameState == STATE_NORMAL)
        {
        u8 player = m_whiteMove ? WHITE: BLACK;
        if (m_board.isCheckmate (player))
            {
            gameState = STATE_CHECKMATE;
            winner = m_whiteMove ? BLACK: WHITE; // Reversed
            }
        else if (m_board.isStalemate (player))
            {
            gameState = STATE_STALEMATE_NO_MOVES; // TODO
            }
        else 
            {
            Player* p = m_players [player];
            if (p)
                {
                Move move = p->getMove ( m_board );
                bool isMoveLegal = m_board.doMove ( move );
                
                if (!isMoveLegal)
                    throw std::runtime_error ("player attempted an illegal move");
                }
            }
        m_whiteMove = !m_whiteMove;
        }
    if (m_canvas) 
        {
        draw ();
        }
    return gameState == STATE_NORMAL;
    }