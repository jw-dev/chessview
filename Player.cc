#include "Player.h"

// Eval Player

auto EvalPlayer::getMove (Board& board) const -> Move 
    {
    const std::vector<Move> moves = board.getMoves (color);
    std::vector<Move> selected;
    u32 maxScore = 0;
    for (const auto& move: moves)
        {
        u32 score = board.tryMove (move, 
            [this, &board] ()
                { 
                return evalBoard (board); 
                });
        if (score >= maxScore) 
            {
            if (score > maxScore) 
                {
                maxScore = score;
                selected.clear();
                }
            selected.push_back(move);
            }
        }
    if (selected.size() == 1)   
        return selected[0]; 
    else 
        {
        return getRandom (selected);
        }
    }

auto EvalMovesPlayer::getMove (Board& board) const -> Move 
    {
    const std::vector<Move> moves = board.getMoves (color);
    std::vector<Move> selected;
    u32 maxScore = 0;
    for (const auto& move: moves)
        {
        u32 score = evalMove (move);
        if (score >= maxScore) 
            {
            if (score > maxScore) 
                {
                maxScore = score;
                selected.clear();
                }
            selected.push_back(move);
            }
        }
    return getRandom (selected);
    }

auto EvalPositionPlayer::getMove (Board& board) const -> Move 
    {
    const std::vector<Move> moves = board.getMoves ( color );
    std::vector <Move> selected;
    u32 maxScore = 0;
    for (const auto& move: moves)
        {
        // const u8 piece = board.pieceAt ( move.fromCol, move.fromRow );
        // const u32 score = evalPiece ( piece, move.toCol, move.toRow );
        u32 score = 
            board.tryMove(move, 
            [&board, &move, &selected, &maxScore, this] () -> u32
                {
                u32 thisScore = 0;
                for (int c = 0; c < Board::GRID_LENGTH; ++c) 
                    for (int r = 0; r < Board::GRID_LENGTH; ++r)
                        {
                        u8 piece = board.pieceAt (c, r);
                        if (piece && (piece & Board::COLOR_MASK) == color) 
                            {
                            thisScore += evalPiece (piece, c, r);
                            }
                        }
                return thisScore;
                });
        if (score >= maxScore) 
            {
            if (score > maxScore) 
                {
                maxScore = score;
                selected.clear();
                }
            selected.push_back (move);
            }
        }
    return getRandom (selected);
    }

auto EvalPiecePlayer::getMove (Board& board) const -> Move 
    {
    const std::vector<Move> moves = board.getMoves ( color );
    std::vector <Move> selected;
    u32 maxScore = 0;
    for (const auto& move: moves)
        {
        const u8 piece = board.pieceAt ( move.fromCol, move.fromRow );
        const u32 score = evalPiece ( piece, move.toCol, move.toRow );
        if (score >= maxScore) 
            {
            if (score > maxScore) 
                {
                maxScore = score;
                selected.clear();
                }
            selected.push_back (move);
            }
        }
    return getRandom (selected);
    }




// Random 

auto RandomPlayer::getMove (Board& board) const -> Move 
    {
    const auto& moves = board.getMoves (color);
    return getRandom (moves);
    }

// White moves

auto WhiteSquares::evalBoard (Board& board) const -> u32 
    {
    int countWhite = 0; 
    for (int c = 0; c < board.GRID_LENGTH; ++c)
        for (int r = 0; r < board.GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r); 
            if (piece != Board::EMPTY && (piece & Board::COLOR_MASK) == color)
                {
                if (board.colorAt(c, r) == WHITE)
                    countWhite++;
                }
            }
    return countWhite;
    }

// Black squares.
auto BlackSquares::evalBoard (Board& board) const -> u32
    {
    int countBlack = 0; 
    for (int c = 0; c < board.GRID_LENGTH; ++c)
        for (int r = 0; r < board.GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r); 

            if (piece != Board::EMPTY && (piece & Board::COLOR_MASK) == color)
                {
                if (board.colorAt(c, r) == BLACK)
                    countBlack++;
                }
            }
    return countBlack;
    }

// Min opponent moves.
auto MinimizeOpponentMoves::evalBoard (Board& board) const -> u32
    {
    u8 oppColor = color == WHITE? BLACK: WHITE;
    return 100 - board.getMoves (oppColor).size ();
    }

// Max opponent moves.
auto MaximizeOpponentMoves::evalBoard (Board& board) const -> u32
    {
    u8 oppColor = color == WHITE? BLACK: WHITE;
    return board.getMoves (oppColor).size ();
    }

// Min self moves.
auto MinimizeOwnMoves::evalBoard (Board& board) const -> u32
    {
    return 100 - board.getMoves (color).size ();
    }

// Max self moves.
auto MaximizeOwnMoves::evalBoard (Board& board) const -> u32
    {
    return board.getMoves (color).size ();
    }

// Defensive.
auto Defensive::evalBoard (Board& board) const -> u32
    {
    u8 pieces = 0;
    u8 piecesAttacked = 0;
    for (int c = 0; c < Board::GRID_LENGTH; ++c) 
        for (int r = 0; r < Board::GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r);
            if (piece && ((piece & Board::COLOR_MASK) == color))
                {
                pieces++;
                if (board.isAttacked (c, r))
                    piecesAttacked++;
                }
            }
    return (100 * pieces) - piecesAttacked;
    }

// Suicidal.
auto Suicidal::evalBoard (Board& board) const -> u32
    {
    u8 pieces = 0;
    u8 piecesAttacked = 0;
    for (int c = 0; c < Board::GRID_LENGTH; ++c) 
        for (int r = 0; r < Board::GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r);
            if (piece && (piece & Board::COLOR_MASK) == color) 
                {
                pieces++;
                if (board.isAttacked (c, r))
                    piecesAttacked++;
                }
            }
    return (UINT32_MAX - (100 * pieces)) + piecesAttacked;
    }

// Pacifist.
auto Pacifist::evalBoard (Board& board) const -> u32
    {
    u8 pieces = 0;
    u8 piecesAttacked = 0;
    u8 enemy = (color == BLACK ? WHITE: BLACK);
    for (int c = 0; c < Board::GRID_LENGTH; ++c) 
        for (int r = 0; r < Board::GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r);
            if (piece && (piece & Board::COLOR_MASK) == enemy) 
                {
                pieces++;
                if (board.isAttacked (c, r))
                    piecesAttacked++;
                }
            }
    return (100 * pieces) - piecesAttacked;
    }

// Offensive.
auto Offensive::evalBoard (Board& board) const -> u32
    {
    u8 pieces = 0;
    u8 piecesAttacked = 0;
    u8 enemy = (color == BLACK ? WHITE: BLACK);
    for (int c = 0; c < Board::GRID_LENGTH; ++c) 
        for (int r = 0; r < Board::GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r);
            if (piece && (piece & Board::COLOR_MASK) == enemy) 
                {
                pieces++;
                if (board.isAttacked (c, r))
                    piecesAttacked++;
                }
            }
    return (UINT32_MAX - (100 * pieces)) + piecesAttacked;
    }


auto ClearPath::evalPiece (u8, u8 column, u8) const -> u8 
    {
    static const std::vector <u32> bonus = { 8, 4, 2, 0, 0, 2, 4, 8 };
    return bonus [column];
    }

auto Centre::evalPiece (u8, u8 column, u8) const -> u8 
    {
    static const std::vector <u32> bonus = { 0, 1, 2, 3, 3, 2, 1, 0 };
    return bonus [column];
    }

auto Aggresive::evalPiece (u8, u8, u8 row) const -> u8 
    {
    static const std::vector <u32> bonus = { 0, 2, 4, 8, 16, 32, 64, 128 };
    return bonus [row];
    }

auto Passive::evalPiece (u8, u8, u8 row) const -> u8 
    {
    static const std::vector <u32> bonus = { 128, 64, 32, 16, 8, 4, 2, 1 };
    return bonus [row];
    }

auto Expensive::evalPiece (u8 piece, u8, u8) const -> u8 
    {
    static const std::map <u8, u8> bonus
        = {
            { PAWN, 1 },
            { BISHOP, 2 },
            { KNIGHT, 2 },
            { ROOK, 3 },
            { CASTLE, 3 },
            { QUEEN, 4 },
            { KING, 5 },
          };
    return bonus.at( piece & Board::TYPE_MASK );
    }

auto Cheap::evalPiece (u8 piece, u8, u8) const -> u8 
    {
    static const std::map <u8, u8> bonus
        = {
            { PAWN, 5 },
            { BISHOP, 4 },
            { KNIGHT, 4 },
            { ROOK, 3 },
            { CASTLE, 3 },
            { QUEEN, 2 },
            { KING, 1 },
          };
    return bonus.at( piece & Board::TYPE_MASK );
    }

auto Near::evalMove (const Move& move) const -> u32
    {
    u32 distance = std::max ( abs(move.fromCol - move.toCol), abs(move.fromRow - move.toRow) );
    return UINT32_MAX - distance;
    }

auto Far::evalMove (const Move& move) const -> u32
    {
    u32 distance = std::max ( abs(move.fromCol - move.toCol), abs(move.fromRow - move.toRow) );
    return distance;
    }

std::unique_ptr<Player> makeRandom () 
    {
    return std::make_unique<RandomPlayer>();
    }

std::unique_ptr<Player> makeWhiteSquares() 
    {
    return std::make_unique<WhiteSquares>();
    }

std::unique_ptr<Player> makeBlackSquares()
    {
    return std::make_unique<BlackSquares>();
    }

std::unique_ptr<Player> makeMinimizeOpponentMoves()
    {
    return std::make_unique<MinimizeOpponentMoves>();
    }

std::unique_ptr<Player> makeMaximizeOpponentMoves()
    {
    return std::make_unique<MaximizeOpponentMoves>();
    }

std::unique_ptr<Player> makeMinimizeOwnMoves()
    {
    return std::make_unique<MinimizeOwnMoves>();
    }

std::unique_ptr<Player> makeMaximizeOwnMoves()
    {
    return std::make_unique<MaximizeOwnMoves>();
    }

std::unique_ptr<Player> makeDefensive()
    {
    return std::make_unique<Defensive>();
    }

std::unique_ptr<Player> makeOffensive()
    {
    return std::make_unique<Offensive>();
    }

std::unique_ptr<Player> makeSuicidal()
    {
    return std::make_unique<Suicidal>();
    }

std::unique_ptr<Player> makePacifist()
    {
    return std::make_unique<Pacifist>();
    }

std::unique_ptr<Player> makeClearPath()
    {
    return std::make_unique<ClearPath>();
    }

std::unique_ptr<Player> makeCentre() 
    {
    return std::make_unique<Centre>();
    }

std::unique_ptr<Player> makeAggresive()
    {
    return std::make_unique<Aggresive>();
    }

std::unique_ptr<Player> makePassive()
    {
    return std::make_unique<Passive>();
    }

std::unique_ptr<Player> makeExpensive()
    {
    return std::make_unique<Expensive>();
    }

std::unique_ptr<Player> makeCheap()
    {
    return std::make_unique<Cheap>();
    }

std::unique_ptr<Player> makeFar()
    {
    return std::make_unique<Far>();
    }

std::unique_ptr<Player> makeNear()
    {
    return std::make_unique<Near>();
    }