#include "Runner.h"

Runner::Runner ()
  : gameState {STATE_NORMAL},
    winner {0},
    m_players {},
    m_whiteMove {true},
    m_boards {}
    {
    auto &board = m_boards.create ();

    // Pawns 
    for (int row: {1, 6}) 
        for (int column = 0; column < 8; ++column)
            {
            u8 color = row == 1 ? WHITE: BLACK; 
            board.setPiece (PAWN | color, column, row);
            }

    for (int row: {0, 7})
        {
        u8 color = row == 0 ? WHITE: BLACK;
        board.setPiece(QUEEN | color, 3, row);
        board.setPiece(KING | color, 4, row);
        for (int columnMultiplier: {0, 1})
            {
            board.setPiece(BISHOP | color, 2 + (3 * columnMultiplier), row);
            board.setPiece(KNIGHT | color, 1 + (5 * columnMultiplier), row);
            board.setPiece(ROOK | color, 0 + (7 * columnMultiplier), row);
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

auto Runner::board () -> const Board&
    {
    return m_boards.get();
    }

auto Runner::undo () -> void // Undo the last move
    {
    if (!m_boards.atBeg())
        {
        m_boards.move(-1);
        m_whiteMove = !m_whiteMove;
        gameState = STATE_NORMAL;
        }
    }

auto Runner::tick () -> bool
    {
    if (gameState == STATE_NORMAL)
        {
        const u8 player = m_whiteMove ? WHITE: BLACK;
        Board& board = m_boards.get();

        if (board.isCheckmate (player))
            {
            gameState = STATE_CHECKMATE;
            winner = m_whiteMove ? BLACK: WHITE; // Reversed
            }
        else if (board.isStalemate (player))
            {
            gameState = STATE_STALEMATE_NO_MOVES; // TODO
            }
        else 
            {
            if (m_boards.atEnd()) 
                {
                Board& newBoard = m_boards.create();
                Player* p = m_players [ m_whiteMove ? WHITE: BLACK ];
                if (p)
                    {
                    Move move = p->getMove ( newBoard );
                    if (!newBoard.doMove ( move)) 
                        throw std::runtime_error ("player attempted an illegal move");
                    }
                }
            else 
                {
                m_boards.move(1);
                }
            m_whiteMove = !m_whiteMove;
            }
        }
    return gameState == STATE_NORMAL;
    }