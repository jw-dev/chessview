#include "FEN.h"

namespace FEN {
static auto getCastleString(Board *board) -> std::string {
    std::string str = "";
    if (board->canCastle(WHITE, false))
        str.push_back('K');
    if (board->canCastle(WHITE, true))
        str.push_back('Q');
    if (board->canCastle(BLACK, false))
        str.push_back('k');
    if (board->canCastle(BLACK, true))
        str.push_back('q');
    return str == "" ? "-" : str;
}

static auto getEnPassantString(Board *board) -> std::string {
    const u8 column = board->enPassantColumn();
    if (column == 255)
        return "-";

    // at this point an en passant is "possible" (ignoring the fact there may
    // not be a piece available to capture)
    std::string s;
    s.push_back('a' + column);
    s.push_back(board->whiteMove() ? '6' : '3');
    return s;
}

static auto toChar(u8 piece) -> char {
    const char lower = (piece & COLOR_MASK) == BLACK ? 32 : 0;
    switch (piece & TYPE_MASK) {
    case PAWN:
        return 'P' | lower;
    case BISHOP:
        return 'B' | lower;
    case KNIGHT:
        return 'N' | lower;
    case QUEEN:
        return 'Q' | lower;
    case KING:
        return 'K' | lower;
    case CASTLE:
    case ROOK:
        return 'R' | lower;
    default:
        return '?';
    }
}

auto toFEN(Board *board, u16 halfMoveStaleClock, u16 fullMoveclock)
    -> std::string {
    std::ostringstream oss;
    int empty = 0;
    auto flush = [&oss, &empty]() {
        if (empty > 0) {
            oss << empty;
            empty = 0;
        }
    };

    for (u8 row = GRID_LENGTH - 1; row < UINT8_MAX; --row) {
        for (u8 col = 0; col < GRID_LENGTH; ++col) {
            const u8 piece = board->pieceAt(col, row);
            if (piece == EMPTY)
                empty++;
            else {
                flush();
                oss << toChar(piece);
            }
        }
        flush();
        oss << (row ? "/" : " ");
    }
    oss << (board->whiteMove() ? "w " : "b ");
    oss << getCastleString(board) << ' ' << getEnPassantString(board) << ' '
        << halfMoveStaleClock << ' ' << fullMoveclock;
    return oss.str();
}
} // namespace FEN
