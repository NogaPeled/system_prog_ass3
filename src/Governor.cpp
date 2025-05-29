#include "../include/Governor.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Governor::Governor(Game& game, const std::string& name) : Player(game, name) {}
    
    void Governor::tax() {
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        earnCoins(3);
        game.nextTurn();
    }

    void Governor::coup(Player& target) {
        if (getCoins() < 7) throw std::runtime_error("Not enough coins for coup!");
        loseCoins(7);
        target.eliminate();
        game.nextTurn();
    }

    void Governor::undo(Player& target) {
        target.loseCoins(2);
    }
}