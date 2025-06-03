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
}
