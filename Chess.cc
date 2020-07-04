#include "Chess.h"


Chess::Chess (Player *white, Player *black)
    {
    white->setColor (WHITE);
    m_players [WHITE] = white;

    black->setColor (BLACK); 
    m_players [BLACK] = black;

    m_whiteMove = true;
    createDefaultBoard ();
    }


auto Chess::undo () -> void
    {
    if (m_prevStates.empty())
        return; // Nothing to undo

    // Save current state in next states
    m_nextStates.push( snapshot() );

    const GameState &state = m_prevStates.top(); // Make sure to be done with state& before we pop()!
    loadState ( state );
    m_prevStates.pop();
    }

auto Chess::loadState (const GameState& state) -> void 
    {
    m_board.setState (state.boardState);
    m_whiteMove = state.whiteMove;
    winner = state.winner;
    endResult = state.endResult;
    }

auto Chess::snapshot () -> GameState
    {
    return { m_board.getState(), winner, endResult, m_whiteMove };
    }

auto Chess::tick() -> Result 
    {
    if (endResult == None)
        {
        m_prevStates.push ( snapshot() );

        if (!m_nextStates.empty())
            {
            const GameState &state = m_nextStates.top();
            loadState ( state );
            m_nextStates.pop();
            return endResult;
            }

        u8 player = m_whiteMove ? WHITE: BLACK; 
        if (m_board.isCheckmate (player)) 
            {
            endResult = Checkmate;
            winner = m_whiteMove ? BLACK: WHITE; // Reversed; 
            }
        else if (m_board.isStalemate (player)) 
            {
            endResult = Stalemate;
            }
        else 
            {
            Player* p = m_whiteMove ? m_players[WHITE]: m_players[BLACK]; 
            Move move = p->getMove (m_board);
            bool isMoveLegal = m_board.doMove (move);
            
            if (!isMoveLegal)
                throw std::runtime_error ("player attempted an illegal move");
            }
        m_whiteMove = !m_whiteMove;
        }
    return endResult;
    }

auto Chess::createDefaultBoard () -> void 
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
    }