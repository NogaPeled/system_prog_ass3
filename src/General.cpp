#include "../include/General.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    General::General(Game& game, const std::string& name) : Player(game, name) {}

    void General::blockCoup() {
        if (getCoins() < 5) throw std::runtime_error("Not enough coins to block a coup");
        loseCoins(5);
    }

    void General::onArrest() {
        earnCoins(1);
    }
    
    void General::coup(Player& target) {
        if (getCoins() < 7) throw std::runtime_error("Not enough coins for coup.");
        loseCoins(7);
        target.eliminate();
        game.nextTurn();
    }

    void General::undo(Player& target) {
        throw std::runtime_error("General cannot undo actions");
    }
}
