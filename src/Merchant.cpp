//nogapeled19@gmail.com
#include "../include/Merchant.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Merchant::Merchant(Game& game, const std::string& name) : Player(game, name) {} // Constructor: initialize a Merchant player

    void Merchant::onStartTurn() { // At the start of the turn, grant a bonus coin if the Merchant has at least 3
        if (getCoins() >= 3) earnCoins(1);
    }

    void Merchant::onArrest() // When arrested, lose 2 coins to the bank instead of transferring 1 to the attacker
    { 
        if (getCoins() < 2) throw std::runtime_error("Merchant has less than 2 coins to lose");
        loseCoins(2); // The attacker does not get anything — effect is absorbed
    }

}