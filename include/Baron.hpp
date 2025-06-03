#pragma once
#include "Player.hpp"

namespace coup {

    class Baron : public Player {
        public:
        Baron(Game& game, const std::string& name);
        
        void invest();
        void onSanction(); // special reaction when sanctioned
    };
}