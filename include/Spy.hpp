#pragma once
#include "Player.hpp"

namespace coup { 
    class Spy : public Player {
        private:
        Player* arrestedBlockedTarget = nullptr;

        public:
        Spy(Game& game, const std::string& name);

        void watch(Player& target);
        void preventNextArrest(Player& target);

        void coup(Player& target) override;
        void undo(Player& target) override;
    };
}