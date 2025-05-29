#include "../include/Baron.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Baron::Baron(Game& game, const std::string& name) : Player(game, name) {}

    void Baron::invest() {
        enforceCoupRule();
        if (getCoins() < 3) throw std::runtime_error("Not enough coins to invest.");
        loseCoins(3);
        earnCoins(6);
        // Doesnt end the tuen, like bribe
    }

    void Baron::coup(Player& target) {
        if (getCoins() < 7) throw std::runtime_error("Not enough coins for coup.");
        loseCoins(7);
        target.eliminate();
        game.nextTurn();
    }

    void Baron::undo(Player& target) {
        throw std::runtime_error("Baron cannot undo any actions.");
    }

    void Baron::onSanction() {
        earnCoins(1);
    }
}