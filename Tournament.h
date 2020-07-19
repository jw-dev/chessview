#include <map>
#include <string>
#include <functional>
#include "Runner.h" 

using PlayerCreator = std::function<std::unique_ptr<Player>()>;

struct Tournament final 
    {
    Tournament (const std::map <std::string, PlayerCreator> &players);
    auto run() -> void;
private:
    const std::map<std::string, PlayerCreator>& m_players;
    }; 

Tournament::Tournament (const std::map <std::string, PlayerCreator> &players) 
  : m_players { players } 
    {

    }

auto Tournament::run() -> void
    {
    static constexpr unsigned RUN_COUNT = 500;

    for (const auto& white_f: m_players)
        {
        for (const auto& black_f: m_players)
            {
            std::map<u8, unsigned short> wins = {};
            std::shared_ptr <Player> white = white_f.second(),
                                     black = black_f.second();
            RunnerStd runner { white, black };

            for (unsigned int i = 0; i < RUN_COUNT; ++i)
                {
                runner.reset();
                runner.run();

                if (runner.gameState == STATE_CHECKMATE)
                    {
                    wins [runner.winner]++;
                    }
                }
            std::cout << white_f.first 
                      << " v " 
                      << black_f.first 
                      << ": white("
                      << wins[WHITE]
                      << ") black("
                      << wins[BLACK]
                      << ") draw(" 
                      << RUN_COUNT - (wins[WHITE] + wins[BLACK])
                      << ")"
                      << std::endl;
            }
        }
    }