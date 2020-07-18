#include <map>
#include <string>
#include <functional>
#include "Runner.h" 

struct Tournament final 
    {
    Tournament (const std::map <std::string, std::function<Player*()>> &players);
    auto run() -> void;
private:
    const std::map<std::string, std::function<Player*()>>& m_players;
    }; 

Tournament::Tournament (const std::map <std::string, std::function<Player*()>> &players) 
  : m_players { players } 
    {

    }

auto Tournament::run() -> void
    {
    for (const auto& white_f: m_players)
        {
        for (const auto& black_f: m_players)
            {
            int white_win = 0, black_win = 0, draw = 0;

            std::cout << white_f.first << " v " << black_f.first << std::endl;
            
            Player* white = white_f.second(), *black = black_f.second();

            for (int i = 0; i < 50; ++i)
                {
                RunnerStd runner { white, black };
                // runner.reset();
                runner.run();
                switch (runner.gameState)
                    {
                    case STATE_CHECKMATE:
                        if (runner.winner == WHITE) 
                            white_win++;
                        else 
                            black_win++;
                        break;
                    case STATE_STALEMATE_INSUFFICIENT_MATERIAL:
                    case STATE_STALEMATE_NO_MOVES:
                    case STATE_STALEMATE_STALE_MOVES:
                        draw++;
                        break;
                    }   
                }
            std::cout << "white(" << white_win << ") black(" << black_win << ") draw (" << draw << ")" << std::endl;        

            // delete black;
            }
        }
    }