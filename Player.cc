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

auto ClearPath::evalBoard (Board& board) const -> u32
    {
    static const std::vector <u32> penalty = { 0, 2, 4, 8, 8, 4, 2, 0 };
    u32 sum = 0;
    for (int c = 0; c < Board::GRID_LENGTH; ++c)
        for (int r = 0; r < Board::GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r);
            if (piece && (piece & Board::COLOR_MASK) == color) 
                {
                sum += penalty [c];
                }
            }
    return UINT32_MAX - sum;
    }

auto Centre::evalBoard (Board& board) const -> u32
    {
    static const std::vector <u32> bonus = { 0, 2, 4, 8, 8, 4, 2, 0 };
    u32 sum = 0;
    for (int c = 0; c < Board::GRID_LENGTH; ++c)
        for (int r = 0; r < Board::GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r);
            if (piece && (piece & Board::COLOR_MASK) == color) 
                {
                sum += bonus [c];
                }
            }
    return sum;
    }

auto Aggresive::evalBoard (Board& board) const -> u32
    {
    static const std::vector <u32> bonus = { 0, 2, 4, 6, 8, 10, 12, 14 };
    u32 sum = 0;
    for (int c = 0; c < Board::GRID_LENGTH; ++c)
        for (int r = 0; r < Board::GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r);
            if (piece && (piece & Board::COLOR_MASK) == color) 
                {
                sum += bonus [(color == BLACK? Board::GRID_LENGTH-r: r)];
                }
            }
    return sum;
    }

auto Passive::evalBoard (Board& board) const -> u32
    {
    static const std::vector <u32> penalty = { 0, 2, 4, 6, 8, 10, 12, 14 };
    u32 sum = 0;
    for (int c = 0; c < Board::GRID_LENGTH; ++c)
        for (int r = 0; r < Board::GRID_LENGTH; ++r) 
            {
            u8 piece = board.pieceAt (c, r);
            if (piece && (piece & Board::COLOR_MASK) == color) 
                {
                sum += penalty [(color == BLACK? Board::GRID_LENGTH-r: r)];
                }
            }
    return UINT32_MAX - sum;
    }



Player* makeRandom () 
    {
    return new RandomPlayer ();
    }

Player* makeWhiteSquares() 
    {
    return new WhiteSquares ();
    }

Player* makeBlackSquares()
    {
    return new BlackSquares();
    }

Player* makeMinimizeOpponentMoves()
    {
    return new MinimizeOpponentMoves();
    }

Player* makeDefensive()
    {
    return new Defensive();
    }

Player* makeOffensive()
    {
    return new Offensive();
    }

Player* makeSuicidal()
    {
    return new Suicidal();
    }

Player* makePacifist()
    {
    return new Pacifist();
    }

Player* makeClearPath()
    {
    return new ClearPath();
    }

Player* makeCentre() 
    {
    return new Centre();
    }

Player* makeAggresive()
    {
    return new Aggresive();
    }

Player* makePassive()
    {
    return new Passive();
    }