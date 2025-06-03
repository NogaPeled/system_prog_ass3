#include "../include/Governor.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Governor::Governor(Game& game, const std::string& name) : Player(game, name) {}
    
    void Governor::tax() {
        enforceCoupRule();
        if (game.turn() != getName()) throw std::runtime_error("Not your turn!");
        if (isUnderSanction()) throw std::runtime_error("You are under sanction and cannot tax.");

        earnCoins(3);
        game.nextTurn();
    }
}