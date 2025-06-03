#include "../include/Spy.hpp"
#include "../include/Game.hpp"
#include <iostream>
#include <stdexcept>

namespace coup {
    Spy::Spy(Game& game, const std::string& name) : Player(game, name) {}

    void Spy::watch(Player& target) {
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
}