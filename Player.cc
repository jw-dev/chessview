#include "Player.h"

// List of available players
static const std::map <std::string, std::function<Player*()>>
    players = {
                { "random",         makeRandom },
                { "whitesquares",   makeWhiteSquares },
                { "blacksquares",   makeBlackSquares },
                { "minimize",       makeMinimizeOpponentMoves },
                { "defensive",      makeDefensive },
                { "offensive",      makeOffensive },
                { "suicidal",       makeSuicidal },
                { "pacifist",       makePacifist },
                { "clearpath",      makeClearPath },
                { "centre",         makeCentre },
                { "aggressive",     makeAggresive },
                { "passive",        makePassive },
              };

// Helper function to create a player from their name
auto makePlayer (const std::string& arg) -> Player* 
    {
    if ( players.find (arg) != players.end() )
        {
        return players.at (arg) ();
        }
    else 
        {
        std::cerr << "Unknown player: " << arg << " (assuming random)" << std::endl;
        return makeRandom ();
        }
    }

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

auto EvalPiecePlayer::getMove (Board& board) const -> Move 
    {
    const std::vector<Move> moves = board.getMoves ( color );
    std::vector <Move> selected;
    u32 maxScore = 0;
    for (const auto& move: moves)
        {
        u32 score = board.tryMove(move, 
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