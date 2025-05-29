#pragma once
#include "Player.hpp"

namespace coup {
    class General : public Player {
        public:
        General(Game& game, const std::string& name);

        void blockCoup();
        void onArrest();
        void coup(Player& target) override;
        void undo(Player& target) override;
    };
}