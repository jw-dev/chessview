#include "Board.h"


Board::Board () 
  : m_pieces { }
    {
    // Pack vector storage.
    m_pieces.resize (GRID_LENGTH);
    m_pieces.shrink_to_fit ();
    }

Board::Board (const Board& other)
  : m_pieces { }
    {
    std::copy (other.m_pieces.begin(), other.m_pieces.end(), std::back_inserter (m_pieces));
    m_blackStaleMoves = other.m_blackStaleMoves;
    m_whiteStaleMoves = other.m_whiteStaleMoves;
    lastMove = other.lastMove;
    }

auto Board::pieceAt (u8 column, u8 row) const -> u8
    {
    // If we have a black pawn at column 1, then the row will look like 0b[...]00010000.
    // Shifting right by 1 * 4 = 4 gives us 0b[...]00000001.
    // Then bitwise AND with 0b1111 to give us 0b0001. 
    return (m_pieces[row] >> (column * 4)) & PIECE_MASK;
    }

auto Board::colorAt (u8 column, u8 row) const -> u8
    {
    // Odd numbered tiles are white. (0, 0) is black, (1, 0) is white, and so forth.
    return (column + row) & 1 ? WHITE: BLACK;
    }

auto Board::setPiece (u8 piece, u8 column, u8 row) -> void
    {
    // If piece is a black pawn (1) at column 2, then shift 0b0001 left by 2 * 4 = 8. 
    // Mask then arrives at 0b000100000000.
    u32 mask = PIECE_MASK << (column * 4); 
    u32 shift = piece << (column * 4);
    // NOT on the mask, to clear this entire piece in the row.
    m_pieces[row] &= ~mask;
    // OR on the shift, to set the new piece.
    m_pieces[row] |= shift;
    }

auto Board::removePiece (u8 column, u8 row) -> void
    {
    // Mask is set to PIECE_MASK (0b1111) shifted by column * 4. So if column = 2 then mask = 0b111100000000.
    const u32 mask = PIECE_MASK << (column * 4);
    // Clear this mask in the row to wipe the piece.
    m_pieces [row] &= ~mask;
    }

auto Board::inBounds (u8 column, u8 row) const -> bool
    {
    return column < GRID_LENGTH && row < GRID_LENGTH;
    }

auto Board::inBounds (const Move& move) const -> bool
    {
    // Both source and target tile must be in bounds, otherwise the move is illegal.
    return inBounds (move.fromCol, move.fromRow) && inBounds (move.toCol, move.toRow);
    }

auto Board::isMoveLegal (const Move& move) -> bool
    {
    // Move not in bounds, so is immediately illegal
    if ( !inBounds (move) )
        return false;

    // Move that does nothing 
    else if (move.fromCol == move.toCol && move.fromRow == move.toRow) 
        return false;

    // Check source
    u8 piece = pieceAt (move.fromCol, move.fromRow);    
    if (piece == EMPTY) // No piece at source, so nothing to move 
        return false;
    else if ((piece & COLOR_MASK) != move.color) // Piece at source cannot be moved by the provided player
        return false;
    else if ((piece & TYPE_MASK) != PAWN && move.promotion) // Promoting a non pawn
        return false;

    // Check target
    u8 target = pieceAt (move.toCol, move.toRow); 
    if (target && (target & COLOR_MASK) == move.color) // Capture is against same color piece
        return false;
    // Capture is against the King (technically the king cannot be captured)
    else if ((target & TYPE_MASK) == KING) 
        return false;

    return isMoveLegalForPiece (piece, move) && !isMoveIntoCheck (move);
    }

auto Board::isMoveLegalForPiece (u8 piece, const Move& move) -> bool
    {
    // Can't move nothing
    if (piece == EMPTY) 
        return false;

    const u8 dcol = abs (move.fromCol - move.toCol);
    const u8 drow = abs (move.fromRow - move.toRow);

    // Returns -1 if the direction is to the left or down, or 1 otherwise
    auto getDir = 
        [] (u8 from, u8 to) -> int 
            {
            return from < to? 1: -1;
            };

    // Returns whether the piece is blocked on the diagonal vector.
    // Walks all tiles along the diagonal vector and checks for a piece in the way.
    // Note that we cannot "move through" an opponent's piece and capture on the way - we get blocked by any piece.
    auto isBlockedOnDiagonal =
        [&] () -> bool 
            {
            int columnDir = getDir (move.fromCol, move.toCol);
            int rowDir = getDir (move.fromRow, move.toRow);
            // Check every tile on this diagonal vector until we reach our target.
            for (int col = move.fromCol + columnDir, row = move.fromRow + rowDir; col != move.toCol && row != move.toRow; col += columnDir, row += rowDir)
                {
                if ( pieceAt (col, row) )
                    return true;
                }
            return false;
            };

    // Returns whether the piece is blocked on the vertical/horizontal vector.
    // Walks all tiles along the vertical or horizontal vector and checks for a piece in the way.
    // Note that we cannot "move through" an opponent's piece and capture on the way - we get blocked by any piece.
    auto isBlockedOnAxis = 
        [&] () -> bool
            {
            if (drow) 
                { // Moving vertically 
                int dir = getDir (move.fromRow, move.toRow);
                for (int row = move.fromRow + dir; row != move.toRow; row += dir)
                    if ( pieceAt (move.fromCol, row )) 
                        return true;
                }
            else 
                { // Moving horizontally
                int dir = getDir (move.fromCol, move.toCol);
                for (int col = move.fromCol + dir; col != move.toCol; col += dir) 
                    if ( pieceAt (col, move.fromRow) )
                        return true; 
                }
            return false;
            };

    // Not moving
    if (!dcol && !drow) 
        return false;

    switch (piece & TYPE_MASK)
        {
        case BISHOP: 
            // Bishop can only move diagonally, so the delta row MUST equal the delta column (otherwise we're moving staggered)
            return dcol == drow && !isBlockedOnDiagonal ();
        case KING: 
            // King can only move one tile at a time, but in any direction
            return dcol <= 1 && drow <= 1;
        case KNIGHT: 
            // Knight can only move in an L shape; one of the directions must be 2 and one must be 1, exactly.
            return (dcol == 2 && drow == 1) || (dcol == 1 && drow == 2);
        case PAWN:
            {
            // Bit more complex for pawns.
            // Pawns can only move diagonally when capturing, and only 2 tiles from the initial square.

            const u8 color = piece & COLOR_MASK; 
            const int dir = getDir (move.fromRow, move.toRow); 
            if (color == WHITE && dir == -1) // White moving backwards
                return false;
            else if (color == BLACK && dir == 1) // Same for black
                return false;

            if (move.promotion) // Attempting to promote a pawn - is it legal?
                {
                if (move.toRow != ((color == WHITE) ? GRID_LENGTH - 1: 0))
                    return false;
                const u8 pro = move.promotion & TYPE_MASK;
                if (pro != KNIGHT && pro != QUEEN && pro != BISHOP && pro != ROOK)
                    return false;
                }

            if (dcol) // Attacking
                {
                // Can only attack one square diagonally, and can only attack if there is a target
                return dcol == 1 && drow == 1 && pieceAt (move.toCol, move.toRow) != EMPTY;
                }
            
            // Not attacking, so we can only move one or two spaces 
            if (drow > 2) 
                return false; 

            // Can only move two spaces if moving from initial square 
            // For white, this is row 1; for black, this is row 6; (0 based)
            if (drow == 2 && ((color == WHITE && move.fromRow != 1) || (color == BLACK && move.fromRow != 6)))
                return false;

            // Not attacking, so if there is a piece at target then this is an illegal move
            u8 target = pieceAt (move.toCol, move.toRow);
            if (target != EMPTY) 
                return false; 
            else if (drow == 2 && pieceAt (move.fromCol, move.fromRow + dir) != EMPTY)
                return false;

            return true;
            }
        case QUEEN: 
            // If moving diagonally, can only move row and column being the same (straight diagonal)
            if (drow && dcol && drow != dcol) 
                return false;
            return (drow && dcol)? !isBlockedOnDiagonal(): !isBlockedOnAxis(); 
        case ROOK:
            // Rook can only move sideways or vertically, so delta row must be 0 or delta column must be 0, otherwise we are moving diagonally
            return (!drow || !dcol) && !isBlockedOnAxis();
        default:
            return false;
        }
    }

auto Board::isMoveIntoCheck (const Move& move) -> bool
    {
    // Use tryMove to just perform the move and see if we're in check.
    bool inCheckPostMove = tryMove (
        move, [this, &move] () 
            {
            return isCheck (move.color);
            });
    return inCheckPostMove;
    }

auto Board::doMove (const Move& move) -> bool
    {
    // Check if the move is legal and then do it if it is.
    bool moveLegal = isMoveLegal (move);
    if (moveLegal)
        forceDoMove (move); 
    return moveLegal;
    }

auto Board::isCheckmate (u8 color) -> bool
    {
    // Checkmate: player is in check and has no moves remaining.
    bool checkMate = isCheck (color) && hasZeroMoves (color);
    // if (checkMate) 
    //     std::cout << "Checkmate : " << (color == WHITE? "Black" : "White") << " wins" << std::endl; //DEBUG
    return checkMate;
    }

auto Board::isStalemate (u8 color) -> bool
    {
    // Stalemate 1: Player has no legal moves remaining, and is not in check.
    if (!isCheck (color) && hasZeroMoves (color))
        {
        // std::cout << "Stalemate : zero moves." << std::endl; //DEBUG
        return true; 
        }
        

    // Stalemate 2: There has been 0 captures or pawn moves in the last 50 moves.
    if (m_blackStaleMoves >= 50 && m_whiteStaleMoves >= 50)
        {
        // std::cout << "Stalemate : 50 stale moves." << std::endl;
        return true;
        }
    
    // Stalemate 3: if the players have insufficient material, then we cannot checkmate, and the result must be a draw.
    std::vector<u8> types; 
    for (u8 c = 0; c < GRID_LENGTH; ++c) 
        for (u8 r = 0; r < GRID_LENGTH; ++r)
            {
            u8 type = pieceAt ( c, r ) & Board::TYPE_MASK;
            if (type != KING) 
                types.push_back ( type );
            }
    switch (types.size()) 
        {
        case 0: // King vs King 
            // std::cout << "Stalemate : two kings" << std::endl;
            return true;
        case 1: // King vs King and Bishop or King vs King and Knight
            u8 type = types[0]; 
            if (type == BISHOP || type == KNIGHT) 
                {
                // std::cout << "Stalemate : insufficient material." << std::endl;
                return true;
                }
            break;
        }
    return false;
    }

auto Board::forceDoMove(const Move & move) -> void 
    {
    u8 src = pieceAt (move.fromCol, move.fromRow);

    // Check for stale moves. 
    u32& moves = (move.color == WHITE? m_whiteStaleMoves: m_blackStaleMoves);
    if ((src & TYPE_MASK) != PAWN && pieceAt(move.toCol, move.toRow) == EMPTY)
        moves++;
    else    
        moves = 0;


    // Do promotion
    if ((src & TYPE_MASK) == PAWN && move.promotion) 
        {
        const u8 newType = move.promotion & TYPE_MASK;
        const u8 color = src & COLOR_MASK;
        setPiece (newType | color, move.toCol, move.toRow);
        }
    else 
        {
        setPiece (src, move.toCol, move.toRow);
        }
    removePiece (move.fromCol, move.fromRow);
    lastMove = move;
    }

auto Board::hasZeroMoves (u8 color) -> bool
    {
    auto moves = getMoves (color, 1);
    return moves.empty();
    }

auto Board::hasOneMove (u8 color) -> bool
    {
    auto moves = getMoves (color, 2);
    return moves.size() == 1;
    }

auto Board::getMoves (u8 color, u8 count) -> std::vector<Move>
    {
    std::vector <Move> moves; 


    // Loop over all squares and find our pieces.
    for (u8 row = 0; row < GRID_LENGTH; ++row) 
        for (u8 column = 0; column < GRID_LENGTH; ++column) 
            {
            // Adds a move but only if it is legal.
            // Required because for loops below will try to add lots of out-of-bounds and weird, illegal moves.
            auto tryAddMove = 
                [&column, &row, &moves, &color, this] 
                    (u8 toCol, u8 toRow, u8 promote=0U) 
                        {
                        const Move move { column, toCol, row, toRow, color, promote };
                        if ( isMoveLegal (move) )
                            moves.push_back (move);
                        };

            const u8 piece = pieceAt (column, row);
            if ((piece & COLOR_MASK) == color) 
                {
                switch (piece & TYPE_MASK)
                    {
                    case BISHOP:
                        {
                        // Check all diagonal vectors 
                        for (int c: {-1, 1})
                            for (int r: {-1, 1})
                                for (int col = column + c, rw = row + r; inBounds (col, rw); col += c, rw += r)
                                    tryAddMove (col, rw);
                        break;
                        }
                    case KING:
                        {
                        // For King, we just need to try and move to all 8 surrounding tiles
                        for (int c: {-1, 0, 1})
                            for (int r: {-1, 0, 1})
                                tryAddMove (column + c, row + r);
                        break;
                        }
                    case KNIGHT:
                        {
                        // For knight, try all pieces that are delta row = 2 and delta column = 1 or delta row = 1 and delta column = 2.
                        for (int c: {-2, -1, 1, 2}) 
                            for (int r: {-2, -1, 1, 2}) 
                                if ( std::abs (c) != std::abs(r) )
                                    tryAddMove (column + c, row + r);
                        break;
                        }
                    case PAWN:
                        {
                        // All possible pawn moves; +1 row, +2 rows, +1 row +1 column (for capture).
                        u8 color = piece & COLOR_MASK;
                        for (int dc: {-1, 0, 1})
                            for (int dr: {1, 2})
                                {
                                if (dc && dr == 2) continue;

                                // Check for promotion 
                                const u8 newRow = (color == WHITE) ? row + dr: row - dr;
                                const bool isPromote = (newRow == GRID_LENGTH - 1) || newRow == 0; // Black pawns can't ever get to rank 8 and white pawns can't get to rank 0
                                if (isPromote) 
                                    {
                                    for (u8 type: {QUEEN, KNIGHT, ROOK, BISHOP})
                                        tryAddMove (column + dc, newRow, type);
                                    }
                                else 
                                    {
                                    tryAddMove (column + dc, newRow);    
                                    }
                                }
                        break;
                        }
                    case QUEEN:
                        {
                        // Queen, we need to check all diagnoal vectors + the horizontal and vertical axis
                        for (int c: {-1, 0, 1})
                            for (int r: {-1, 0, 1})
                                if (c || r) 
                                    for (int col = column + c, rw = row + r; inBounds(col, rw); col += c, rw += r)
                                        tryAddMove (col, rw);
                        break;
                        }
                    case ROOK:
                        {
                        // Try horizontal and vertical vectors.
                        for (int mult: {-1, 1})
                            {
                            for (int col = column + mult; inBounds(col, row); col += mult)
                                tryAddMove (col, row); 
                            for (int rw = row + mult; inBounds(column, rw); rw += mult)
                                tryAddMove (column, rw);
                            }
                        break;
                        }
                    }
                // Break out if we've hit the move limit.
                if (count && moves.size() >= count)
                    {
                    return moves;
                    }
                }
            }
    return moves; 
    }

auto Board::isCheck (u8 color) -> bool
    {
    const u8 myKing = (KING | (color & COLOR_MASK));
    // Find king.
    for (u8 row = 0; row < GRID_LENGTH; ++row)
        for (u8 column = 0; column < GRID_LENGTH; ++column) 
            {
            if ( pieceAt (column, row) == myKing )
                {
                // Return whether king is attacked. Here, we assume there can be only one king per player, which makes sense.
                // But worth bearing in mind in case we start creating weird games/puzzles with multiple kings.
                return isAttacked (column, row);
                // For multiple kings:
                // if (isAttacked (column, row)) return true;
                }
            }
    return false;
    }

auto Board::isAttacked (u8 column, u8 row) const -> bool
    {
// 2300ms
#if 0 
    const u8 piece = pieceAt ( column, row ); 
    if (!piece)
        return false;

    const u8 attackerColor = ((piece & COLOR_MASK) == WHITE)? BLACK : WHITE; 
    for (u8 r = 0; r < GRID_LENGTH; ++r) 
        {
        if ( !m_pieces[r] )
            continue; 
        for (u8 c = 0; c < GRID_LENGTH; ++c) 
            {
            u8 piece = pieceAt ( c, r );
            if (piece && (piece & COLOR_MASK) == attackerColor)
                {
                Move move { c, column, r, row, attackerColor };
                if ( isMoveLegalForPiece ( piece, move ) )
                    return true;
                }
            }
        }
    return false;
#endif
// 1750ms
#if 1
    const u8 piece = pieceAt (column, row);
    const u8 attackerColor = (piece & COLOR_MASK) == WHITE? BLACK: WHITE; 

    // First of all, check knight moves 
    // All squares around the tile that are +2 rows, +2 colums. (or -ve)
    const u8 knight = (KNIGHT | attackerColor);
    for (int r: { -1, 1 }) 
        for (int c: { -1, 1 }) 
            {
            if (inBounds(column + c * 2, row + r) && pieceAt (column + c * 2, row + r) == knight)
                return true; 
            else if (inBounds(column + c, row + r * 2) && pieceAt (column + c, row + r * 2) == knight)
                return true;
            }

    for (int colMove: {-1, 0, 1}) 
        for (int rowMove: {-1, 0, 1}) 
            {
            // Skip no move
            if (!colMove && !rowMove) continue;

            // Iterate until we find a piece 
            bool foundPiece = false;

            // Check all tiles on this vector until we find piece, or leave confines of the grid.
            for (u8 c = column + colMove, r = row + rowMove; 
                    c < GRID_LENGTH && r < GRID_LENGTH && !foundPiece; 
                    c += colMove, r += rowMove)
                {
                const u8 attackerPiece = pieceAt (c, r);
                if (!attackerPiece) 
                    continue; // Move to next tile, nothing of interest here

                foundPiece = true;
                if ((attackerPiece & COLOR_MASK) != attackerColor)
                    break; // Blocked by our own piece, so ignore this vector 

                const u8 dcol = abs (c - column);
                const u8 drow = abs (r - row);
                switch (attackerPiece & TYPE_MASK)
                    {
                    case BISHOP:
                        if (dcol && drow && dcol == drow) 
                            return true;
                        break;
                    case KING:
                        if (dcol <= 1 && drow <= 1)
                            return true;
                        break;
                    case PAWN:
                        if (dcol == 1 && rowMove == ((attackerColor & COLOR_MASK) == WHITE? -1: 1))
                            return true;
                        break;
                    case QUEEN:
                        return true;
                    case ROOK: 
                        if ((dcol && !drow) || (!dcol && drow))
                            return true;
                        break;
                    }    
                }
            }
    return false;
#endif
    }