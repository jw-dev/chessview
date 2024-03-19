#ifndef PLAYER_H
#define PLAYER_H

#include "Board.h"
#include "Move.h"
#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <random>

template <typename T> T getRandom(const std::vector<T> &vec) {
    static std::random_device device;
    static std::mt19937 mt{device()};
    std::uniform_int_distribution<> distrib(0, vec.size() - 1);
    return vec[distrib(mt)];
}

struct Player {
    u8 color = 0U;
    virtual ~Player() {}
    virtual auto getMove(Board &) const -> Move = 0;
    inline auto setColor(u8 col) -> void { color = col; };
};

struct EvalPlayer : public Player {
    virtual auto evalBoard(Board &board) const -> u32 = 0;
    auto getMove(Board &board) const -> Move override;
};

// Pure interface for a player who wants to only look at the individual moves.
// This player has no idea of the board state. Useful for when we want to prefer
// certain positions, rather than boards.
struct EvalMovesPlayer : public Player {
    virtual auto evalMove(const Move &move) const -> u32 = 0;
    auto getMove(Board &board) const -> Move override;
};

// Interface for a player who wants to rank the position and type of pieces.
// Player has no idea of the board state or even what move is played to get the
// pieces to those positions.
struct EvalPositionPlayer : public Player {
    virtual auto evalPiece(u8 piece, u8 column, u8 row) const
        -> u8 = 0; // Must return a value 0-255 (higher is better)
    auto getMove(Board &board) const -> Move override;
};

// Interface for a player who wants to rank the position and type of a single
// piece - the one that is about to move. Player has no idea of the board state.
struct EvalPiecePlayer : public Player {
    virtual auto evalPiece(u8 piece, u8 column, u8 row) const
        -> u8 = 0; // Must return a value 0-255 (higher is better)
    auto getMove(Board &board) const -> Move override;

  protected:
    mutable int m_move = {0};
};

struct RandomPlayer : Player {
    auto getMove(Board &) const -> Move override;
};

struct WhiteSquares : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

struct BlackSquares : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

struct MinimizeOpponentMoves : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

struct MinimizeOwnMoves : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

struct MaximizeOpponentMoves : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

struct MaximizeOwnMoves : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

// A player that minimizes the number of own pieces that are under attack.
struct Defensive : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

// A player that maximizes the number of own pieces that are under attack.
struct Suicidal : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

// A player that maximizes the number of enemy pieces that are under attack.
struct Offensive : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

// A player that minimizes the number of enemy pieces that are under attack.
struct Pacifist : EvalPlayer {
    auto evalBoard(Board &board) const -> u32 override;
};

struct ClearPath : EvalPositionPlayer {
    auto evalPiece(u8 piece, u8 column, u8 row) const -> u8 override;
};

struct Centre : EvalPositionPlayer {
    auto evalPiece(u8 piece, u8 column, u8 row) const -> u8 override;
};

struct BongCloud : EvalPiecePlayer {
    auto evalPiece(u8 piece, u8 column, u8 row) const -> u8 override;
};

std::unique_ptr<Player> makeRandom();
std::unique_ptr<Player> makeWhiteSquares();
std::unique_ptr<Player> makeBlackSquares();
std::unique_ptr<Player> makeMinimizeOpponentMoves();
std::unique_ptr<Player> makeMaximizeOpponentMoves();
std::unique_ptr<Player> makeMinimizeOwnMoves();
std::unique_ptr<Player> makeMaximizeOwnMoves();
std::unique_ptr<Player> makeDefensive();
std::unique_ptr<Player> makeOffensive();
std::unique_ptr<Player> makeSuicidal();
std::unique_ptr<Player> makePacifist();
std::unique_ptr<Player> makeCheckmate();
std::unique_ptr<Player> makeClearPath();
std::unique_ptr<Player> makeCentre();
std::unique_ptr<Player> makeBongCloud();

// Defensive (move pieces out of danger)
// Offensive (capture as much as possible)
// Active king (prefer king moves and captures)

#endif