//nogapeled19@gmail.com
#pragma once
#include "Player.hpp"

namespace coup { 

        /**
     * @class Spy
     * @brief Represents the Spy role in the Coup game.
     *
     * The Spy can observe a player and block their next arrest action.
     * This block is automatically consumed if triggered.
     */
    class Spy : public Player {
        private:
        Player* arrestedBlockedTarget = nullptr; // The player whose next arrest will be blocked

        public:

            /**
         * @brief Constructs a Spy and registers them to the game.
         * @param game Reference to the game instance.
         * @param name Name of the player.
         */
        Spy(Game& game, const std::string& name);
        
        /**
         * @brief Mark a player as being watched by the Spy.
         *
         * This enables two effects:
         * 1. The Spy (via the GUI) is shown the target’s current coin count.
         * 2. The target may be blocked from using an arrest on their next turn.
         *
         * Internally, this function stores the watched target.
         * If that player attempts to arrest, the Spy will be prompted (in the GUI)
         * to decide whether to block it, handled via preventNextArrest().
         *
         * Note: Coin visibility is shown in the GUI, not handled here.
         */
        void watch(Player& target);

        /**
         * @brief Called when a player attempts to perform an arrest.
         *
         * If the player is currently being watched by the Spy,
         * the Spy will be prompted via a popup (GUI) to decide whether to block the arrest.
         * If the Spy chooses to block, the arrest is cancelled and the watch is cleared.
         * Otherwise, the arrest proceeds and the watch is still cleared.
         */
        void preventNextArrest(Player& target);

            /**
         * @brief Clears the currently watched player (Resets the spy’s tracked target)
         */
        void clearWatch(); 

            /**
         * @brief Returns the player whose arrest is currently being blocked.
         * @return Pointer to the blocked player, or nullptr if none.
         */
        Player* getBlockedTarget() const;
    };
}