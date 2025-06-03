#include "../include/Judge.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Judge::Judge(Game& game, const std::string& name) : Player(game, name) {}

    void Judge::undoBribe(Player& target) {
        // The target loses the 4 coins spent on bribe and loses their extra move
        target.resetActions();
    }

    void Judge::onSanctionedBy(Player& attacker) {
        attacker.loseCoins(1); // Attacker punished for sanctioning the judge
    }
}