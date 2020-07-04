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
    virtual auto name () const -> std::string = 0;
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
    auto name () const -> std::string override;
    };

struct WhiteSquares: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    auto name () const -> std::string override;
    };

struct BlackSquares: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    auto name () const -> std::string override;
    };

struct MinimizeOpponentMoves: EvalPlayer 
    {
    auto evalBoard (Board& board) const -> u32 override;
    auto name () const -> std::string override;
    };



Player* makeRandom();
Player* makeWhiteSquares();
Player* makeBlackSquares();
Player* makeMinimizeOpponentMoves();


// Defensive (move pieces out of danger)
// Offensive (capture as much as possible)
// Active king (prefer king moves and captures)


#endif 