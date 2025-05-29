#include "../include/Merchant.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Merchant::Merchant(Game& game, const std::string& name) : Player(game, name) {}

    void Merchant::onStartTurn() {
        if (getCoins() >= 3) earnCoins(1);
    }

    void Merchant::onArrest() {
        if (getCoins() < 2) throw std::runtime_error("Merchant has less than 2 coins to lose");
        loseCoins(2); // The attacker does not get anything — effect is absorbed
    }

    void Merchant::coup(Player& target) {
        if (getCoins() < 7) throw std::runtime_error("Not enough coins for coup");
        loseCoins(7);
        target.eliminate();
        game.nextTurn();
    }

    void Merchant::undo(Player& target) {
        throw std::runtime_error("Merchant cannot undo actions.");
    }

}