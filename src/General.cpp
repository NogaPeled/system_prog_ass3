//nogapeled19@gmail.com
#include "../include/General.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    General::General(Game& game, const std::string& name) : Player(game, name) {} // Constructor: initialize a General player

    void General::blockCoup() { // Pay 5 coins to block a coup
        if (getCoins() < 5) throw std::runtime_error("Not enough coins to block a coup");
        loseCoins(5);
    }

    void General::onArrest() { // When arrested, gain back 1 coin as compensatio
        earnCoins(1);
    }
}
