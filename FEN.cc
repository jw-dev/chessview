#include "FEN.h"

namespace FEN 
    {
    static auto canCastle ( Board* board, bool isWhite, bool isQueenside ) -> bool 
        {
        u8 color = isWhite ? WHITE : BLACK;
        u8 row = isWhite ? 0 : Board::GRID_LENGTH - 1;
        u8 column = isQueenside ? 0 : Board::GRID_LENGTH - 1;
        return !board->kingMoved ( color ) && ( board->pieceAt ( column, row ) & Board::TYPE_MASK ) == CASTLE;
        }

    static auto getCastleString ( Board* board ) -> std::string
        {
        std::string str = "";
        if ( canCastle ( board, true, false  ) ) str.push_back ('K');
        if ( canCastle ( board, true, true   ) ) str.push_back ('Q');
        if ( canCastle ( board, false, false ) ) str.push_back ('k');
        if ( canCastle ( board, false, true  ) ) str.push_back ('q');
        return str == "" ? "-": str;
        }

    static auto getEnPassantString ( Board* board ) -> std::string 
        {
        const u8 lastPiece = board->pieceAt ( board->lastMove.toCol, board->lastMove.toRow );
        const int diff = board->lastMove.fromRow - board->lastMove.toRow;
        if ( (lastPiece & Board::TYPE_MASK) == PAWN && ( diff == -2 || diff == 2 ) )
            {
            // at this point an en passant is "possible" (ignoring the fact there may not be a piece available to capture)
            std::string s;
            s.push_back ( 'a' + board->lastMove.fromCol );
            s.push_back ( diff == 2 ? '6': '3' );
            return s;
            }
        return "-";
        }

    static auto toChar ( u8 piece ) -> char 
        {
        const char lower = ( piece & Board::COLOR_MASK ) == BLACK ? 32 : 0;
        switch ( piece & Board::TYPE_MASK ) 
            {
            case PAWN: return 'P' | lower;
            case BISHOP: return 'B' | lower;
            case KNIGHT: return 'N' | lower;
            case QUEEN: return 'Q' | lower;
            case KING: return 'K' | lower;
            case CASTLE:
            case ROOK: return 'R' | lower;
            default: return '?';
            }
        }
        
    auto toFEN ( Board* board ) -> std::string
        {
        std::ostringstream oss;
        int empty = 0;
        auto flush = [&oss, &empty] () 
            {
            if ( empty > 0) 
                {
                oss << empty;
                empty = 0;
                }
            };

        for ( u8 row = Board::GRID_LENGTH - 1 ; row < UINT8_MAX; --row) 
            {
            for ( u8 col = 0; col < Board::GRID_LENGTH ; ++col )
                {
                const u8 piece = board->pieceAt ( col, row );
                if ( piece == EMPTY )
                    empty ++;
                else 
                    {
                    flush ();
                    oss << toChar ( piece );
                    }
                }
            flush ();
            oss << ( row ? "/" : " " );
            }
        oss << ( board->whiteMove()? "w ": "b " );
        oss << getCastleString ( board )
            << ' '
            << getEnPassantString ( board )
            << ' '
            << board->staleHalfMoveClock ()
            << ' '
            << board->fullMoveClock ();
        return oss.str ();
        }
    }
