#include "../include/Spy.hpp"
#include "../include/Game.hpp"
#include <iostream>
#include <stdexcept>

namespace coup {
    Spy::Spy(Game& game, const std::string& name) : Player(game, name) {}

    void Spy::watch(Player& target) {
        std::cout << name << "(Spy) is watching " << target.getName() << std::endl;
        std::cout << target.getName() << " has " << target.getCoins() << " coins." << std::endl; // Show coin count
        arrestedBlockedTarget = &target; // Block their next arrest action (simplified)
    }

    void Spy::preventNextArrest(Player& actor) {
        if (arrestedBlockedTarget == &actor) {
            arrestedBlockedTarget = nullptr;  // Consume the block
            throw std::runtime_error(actor.getName() + "'s arrest was blocked by Spy!");
        }
    }

    void Spy::clearWatch() {
        arrestedBlockedTarget = nullptr;
    }

    Player* Spy::getBlockedTarget() const {
        return arrestedBlockedTarget;
    }

    void Spy::coup(Player& target)
    {
        if (coins < 7) throw std::runtime_error("Not enough coins for coup!");
        loseCoins(7);
        target.eliminate();
        game.nextTurn();
    }

    void Spy::undo(Player& target) {
        throw std::runtime_error("Spy cannot undo any actions");
    }

}