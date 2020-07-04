#ifndef PLAYER_H
#define PLAYER_H

#include <random>
#include <numeric>
#include "Board.h"
#include "Move.h"


template <typename T> 
T getRandom (const std::vector<T>& vec) 
    {
    static std::random_device device; 
    static std::mt19937 mt { device () };
    std::uniform_int_distribution<> distrib (0, vec.size() - 1);
    return vec [distrib(mt)];
    }

struct DetailedMove 
    {
    Move move; 
    };

struct Player 
    {
    u8 color;
    virtual auto getMove (Board&) const -> Move = 0;
    inline virtual auto setColor (u8 col) -> void { color = col; };
    };

struct EvalPlayer: public Player 
    {
    virtual auto evalBoard (Board& board) const -> u32 = 0;
    virtual auto getMove (Board& board) const -> Move override;
    };

struct RandomPlayer: Player 
    {
    auto getMove (Board&) const -> Move override;
    };

struct WhiteSquares: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    };

struct BlackSquares: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    };

struct MinimizeOpponentMoves: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    };


// A player that minimizes the number of own pieces that are under attack.
struct Defensive: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    };

// A player that maximizes the number of own pieces that are under attack.
struct Suicidal: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    };

// A player that maximizes the number of enemy pieces that are under attack.
struct Offensive: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    };

// A player that minimizes the number of enemy pieces that are under attack.
struct Pacifist: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    };




Player* makeRandom();
Player* makeWhiteSquares();
Player* makeBlackSquares();
Player* makeMinimizeOpponentMoves();
Player* makeDefensive();
Player* makeOffensive();
Player* makeSuicidal();
Player* makePacifist();


// Defensive (move pieces out of danger)
// Offensive (capture as much as possible)
// Active king (prefer king moves and captures)


#endif 