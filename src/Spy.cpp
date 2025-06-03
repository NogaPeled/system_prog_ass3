//nogapeled19@gmail.com
#include "../include/Spy.hpp"
#include "../include/Game.hpp"
#include <iostream>
#include <stdexcept>

namespace coup {
    Spy::Spy(Game& game, const std::string& name) : Player(game, name) {} // Constructor: initialize a Spy player

    void Spy::watch(Player& target) {
        arrestedBlockedTarget = &target; // Block their next arrest action (simplified)
    }

    void Spy::preventNextArrest(Player& actor) // Called when a player attempts arrest. The Spy will be prompted to choose whether to block the arrest. If blocked, the watch is cleared
    {
        if (arrestedBlockedTarget == &actor) {
            arrestedBlockedTarget = nullptr;  // Consume the block
            throw std::runtime_error(actor.getName() + "'s arrest was blocked by Spy!");
        }
    }

    void Spy::clearWatch() { // Manually clear the Spy's target (e.g., after turn ends)
        arrestedBlockedTarget = nullptr;
    }

    Player* Spy::getBlockedTarget() const { // Return the currently blocked player, if any
        return arrestedBlockedTarget;
    }
}