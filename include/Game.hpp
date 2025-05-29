#pragma once
#include <vector>
#include <string>

namespace coup {
    class Player;

    class Game {
        private:
        std::vector<Player*> players_list;
        int current_turn_index = 0;

        public:
        void addPlayer(Player* p);
        std::vector<std::string> players() const;
        std::string turn() const;
        void nextTurn();
        std::string winner() const;
        const std::vector<Player*>& getPlayers() const;
    };
}