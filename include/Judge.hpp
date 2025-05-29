#pragma once
#include "Player.hpp"

namespace coup {
    class Judge : public Player {
        public:
        Judge(Game& game, const std::string& name);

        void undoBribe(Player& target); // Cancels the target's bribe
        void onSanctionedBy(Player& attacker); // Penalty when sanctioned
        void coup(Player& target) override;
        void undo(Player& target) override;
    };
}