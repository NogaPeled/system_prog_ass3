#pragma once
#include "Player.hpp"

namespace coup {

    class Baron : public Player {
        public:
        Baron(Game& game, const std::string& name);
        
        void invest();
        void coup(Player& target) override;
        void undo(Player& target) override;
        void onSanction(); // special reaction when sanctioned
    };
}