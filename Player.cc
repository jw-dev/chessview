#include "Player.h"

// Eval Player

auto EvalPlayer::getMove(Board &board) const -> Move {
    const std::vector<Move> moves = board.getMoves(color);
    std::vector<Move> selected;
    u32 maxScore = 0;
    for (const auto &move : moves) {
        u32 score =
            board.tryMove(move, [this, &board]() { return evalBoard(board); });
        if (score >= maxScore) {
            if (score > maxScore) {
                maxScore = score;
                selected.clear();
            }
            selected.push_back(move);
        }
    }
    if (selected.size() == 1)
        return selected[0];
    else {
        return getRandom(selected);
    }
}

auto EvalMovesPlayer::getMove(Board &board) const -> Move {
    const std::vector<Move> moves = board.getMoves(color);
    std::vector<Move> selected;
    u32 maxScore = 0;
    for (const auto &move : moves) {
        u32 score = evalMove(move);
        if (score >= maxScore) {
            if (score > maxScore) {
                maxScore = score;
                selected.clear();
            }
            selected.push_back(move);
        }
    }
    return getRandom(selected);
}

auto EvalPositionPlayer::getMove(Board &board) const -> Move {
    const std::vector<Move> moves = board.getMoves(color);
    std::vector<Move> selected;
    u32 maxScore = 0;
    for (const auto &move : moves) {
        // const u8 piece = board.pieceAt ( move.fromCol, move.fromRow );
        // const u32 score = evalPiece ( piece, move.toCol, move.toRow );
        u32 score = board.tryMove(
            move, [&board, &move, &selected, &maxScore, this]() -> u32 {
                u32 thisScore = 0;
                for (int c = 0; c < GRID_LENGTH; ++c)
                    for (int r = 0; r < GRID_LENGTH; ++r) {
                        u8 piece = board.pieceAt(c, r);
                        if (piece && (piece & COLOR_MASK) == color) {
                            thisScore += evalPiece(piece, c, r);
                        }
                    }
                return thisScore;
            });
        if (score >= maxScore) {
            if (score > maxScore) {
                maxScore = score;
                selected.clear();
            }
            selected.push_back(move);
        }
    }
    return getRandom(selected);
}

auto EvalPiecePlayer::getMove(Board &board) const -> Move {
    const std::vector<Move> moves = board.getMoves(color);
    std::vector<Move> selected;
    u32 maxScore = 0;
    for (const auto &move : moves) {
        const u8 piece = board.pieceAt(move.fromCol, move.fromRow);
        const u32 score = evalPiece(piece, move.toCol, move.toRow);
        if (score >= maxScore) {
            if (score > maxScore) {
                maxScore = score;
                selected.clear();
            }
            selected.push_back(move);
        }
    }
    m_move++;
    return getRandom(selected);
}

// Random

auto RandomPlayer::getMove(Board &board) const -> Move {
    const auto &moves = board.getMoves(color);
    return getRandom(moves);
}

// White moves

auto WhiteSquares::evalBoard(Board &board) const -> u32 {
    int countWhite = 0;
    for (int c = 0; c < GRID_LENGTH; ++c)
        for (int r = 0; r < GRID_LENGTH; ++r) {
            u8 piece = board.pieceAt(c, r);
            if (piece != EMPTY && (piece & COLOR_MASK) == color) {
                if (board.colorAt(c, r) == WHITE)
                    countWhite++;
            }
        }
    return countWhite;
}

// Black squares.
auto BlackSquares::evalBoard(Board &board) const -> u32 {
    int countBlack = 0;
    for (int c = 0; c < GRID_LENGTH; ++c)
        for (int r = 0; r < GRID_LENGTH; ++r) {
            u8 piece = board.pieceAt(c, r);

            if (piece != EMPTY && (piece & COLOR_MASK) == color) {
                if (board.colorAt(c, r) == BLACK)
                    countBlack++;
            }
        }
    return countBlack;
}

// Min opponent moves.
auto MinimizeOpponentMoves::evalBoard(Board &board) const -> u32 {
    u8 oppColor = color == WHITE ? BLACK : WHITE;
    return 100 - board.getMoves(oppColor).size();
}

// Max opponent moves.
auto MaximizeOpponentMoves::evalBoard(Board &board) const -> u32 {
    u8 oppColor = color == WHITE ? BLACK : WHITE;
    return board.getMoves(oppColor).size();
}

// Min self moves.
auto MinimizeOwnMoves::evalBoard(Board &board) const -> u32 {
    return 100 - board.getMoves(color).size();
}

// Max self moves.
auto MaximizeOwnMoves::evalBoard(Board &board) const -> u32 {
    return board.getMoves(color).size();
}

// Defensive.
auto Defensive::evalBoard(Board &board) const -> u32 {
    u8 pieces = 0;
    u8 piecesAttacked = 0;
    for (int c = 0; c < GRID_LENGTH; ++c)
        for (int r = 0; r < GRID_LENGTH; ++r) {
            u8 piece = board.pieceAt(c, r);
            if (piece && ((piece & COLOR_MASK) == color)) {
                pieces++;
                if (board.isAttacked(c, r))
                    piecesAttacked++;
            }
        }
    return (100 * pieces) - piecesAttacked;
}

// Suicidal.
auto Suicidal::evalBoard(Board &board) const -> u32 {
    u8 pieces = 0;
    u8 piecesAttacked = 0;
    for (int c = 0; c < GRID_LENGTH; ++c)
        for (int r = 0; r < GRID_LENGTH; ++r) {
            u8 piece = board.pieceAt(c, r);
            if (piece && (piece & COLOR_MASK) == color) {
                pieces++;
                if (board.isAttacked(c, r))
                    piecesAttacked++;
            }
        }
    return (UINT32_MAX - (100 * pieces)) + piecesAttacked;
}

// Pacifist.
auto Pacifist::evalBoard(Board &board) const -> u32 {
    u8 pieces = 0;
    u8 piecesAttacked = 0;
    u8 enemy = (color == BLACK ? WHITE : BLACK);
    for (int c = 0; c < GRID_LENGTH; ++c)
        for (int r = 0; r < GRID_LENGTH; ++r) {
            u8 piece = board.pieceAt(c, r);
            if (piece && (piece & COLOR_MASK) == enemy) {
                pieces++;
                if (board.isAttacked(c, r))
                    piecesAttacked++;
            }
        }
    return (100 * pieces) - piecesAttacked;
}

// Offensive.
auto Offensive::evalBoard(Board &board) const -> u32 {
    u8 pieces = 0;
    u8 piecesAttacked = 0;
    u8 enemy = (color == BLACK ? WHITE : BLACK);
    for (int c = 0; c < GRID_LENGTH; ++c)
        for (int r = 0; r < GRID_LENGTH; ++r) {
            u8 piece = board.pieceAt(c, r);
            if (piece && (piece & COLOR_MASK) == enemy) {
                pieces++;
                if (board.isAttacked(c, r))
                    piecesAttacked++;
            }
        }
    return (UINT32_MAX - (100 * pieces)) + piecesAttacked;
}

auto ClearPath::evalPiece(u8, u8 column, u8) const -> u8 {
    static const std::vector<u32> bonus = {8, 4, 2, 0, 0, 2, 4, 8};
    return bonus[column];
}

auto Centre::evalPiece(u8, u8 column, u8) const -> u8 {
    static const std::vector<u32> bonus = {0, 1, 2, 3, 3, 2, 1, 0};
    return bonus[column];
}

auto BongCloud::evalPiece(u8 piece, u8 column, u8) const -> u8 {
    u8 type = piece & TYPE_MASK;
    u8 ret = 0U;

    if (m_move == 0 && type == PAWN && (column == 3 || column == 4))
        ret = UINT8_MAX;
    else if (m_move == 1 && type == KING)
        ret = UINT8_MAX;
    return ret;
}

std::unique_ptr<Player> makeRandom() {
    return std::make_unique<RandomPlayer>();
}

std::unique_ptr<Player> makeWhiteSquares() {
    return std::make_unique<WhiteSquares>();
}

std::unique_ptr<Player> makeBlackSquares() {
    return std::make_unique<BlackSquares>();
}

std::unique_ptr<Player> makeMinimizeOpponentMoves() {
    return std::make_unique<MinimizeOpponentMoves>();
}

std::unique_ptr<Player> makeMaximizeOpponentMoves() {
    return std::make_unique<MaximizeOpponentMoves>();
}

std::unique_ptr<Player> makeMinimizeOwnMoves() {
    return std::make_unique<MinimizeOwnMoves>();
}

std::unique_ptr<Player> makeMaximizeOwnMoves() {
    return std::make_unique<MaximizeOwnMoves>();
}

std::unique_ptr<Player> makeDefensive() {
    return std::make_unique<Defensive>();
}

std::unique_ptr<Player> makeOffensive() {
    return std::make_unique<Offensive>();
}

std::unique_ptr<Player> makeSuicidal() { return std::make_unique<Suicidal>(); }

std::unique_ptr<Player> makePacifist() { return std::make_unique<Pacifist>(); }

std::unique_ptr<Player> makeClearPath() {
    return std::make_unique<ClearPath>();
}

std::unique_ptr<Player> makeCentre() { return std::make_unique<Centre>(); }

std::unique_ptr<Player> makeBongCloud() {
    return std::make_unique<BongCloud>();
}