//nogapeled19@gmail.com
#include "../include/Judge.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Judge::Judge(Game& game, const std::string& name) : Player(game, name) {} // Constructor: initialize a Judge player

    void Judge::undoBribe(Player& target) // Cancels a player's bribe effect: removes extra action
    {
        // The target loses the 4 coins spent on bribe and loses their extra move
        target.resetActions();
    }

    void Judge::onSanctionedBy(Player& attacker) { // Called when Judge is sanctioned: attacker loses 1 coin for sanctioning the judge
        attacker.loseCoins(1); 
    }
}