#pragma once
#include "Player.hpp"

namespace coup {
    class Merchant : public Player {
        public:
        Merchant(Game& game, const std::string& name);

        void onStartTurn(); // Grant +1 coin if >= 3 at turn start
        void onArrest(); // Special reaction to arrest
    };
}