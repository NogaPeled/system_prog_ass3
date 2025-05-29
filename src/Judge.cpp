#include "../include/Judge.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Judge::Judge(Game& game, const std::string& name) : Player(game, name) {}

    void Judge::undoBribe(Player& target) {
        // The target loses the 4 coins spent on bribe and loses their extra move
        target.loseCoins(4);
    }

    void Judge::onSanctionedBy(Player& attacker) {
        attacker.loseCoins(1); // Attacker punished for sanctioning the judge
    }
    
    void Judge::coup(Player& target) {
        if (getCoins() < 7) throw std::runtime_error("Not enough coins for coup");
        loseCoins(7);
        target.eliminate();
        game.nextTurn();
    }

    void Judge::undo(Player& target)
    {
        throw std::runtime_error("Judge cannot undo any actions.");
    }
}