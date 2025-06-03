//nogapeled19@gmail.com
#include "../include/Baron.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Baron::Baron(Game& game, const std::string& name) : Player(game, name) {} // Constructor: initialize a Baron player

    void Baron::invest() // Invest 3 coins to gain 6 coins
    {
        enforceCoupRule(); // Must coup if holding 10+ coins
        if (getCoins() < 3) throw std::runtime_error("Not enough coins to invest.");
        loseCoins(3);
        earnCoins(6);
        // Does not end the turn, like bribe
    }

    void Baron::onSanction() { // Called when the Baron is sanctioned — gain 1 coin as compensation
        earnCoins(1);
    }
}