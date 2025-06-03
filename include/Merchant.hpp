//nogapeled19@gmail.com
#pragma once
#include "Player.hpp"

namespace coup {

        /**
     * @class Merchant
     * @brief Represents the Merchant role in the Coup game.
     *
     * The Merchant gains a bonus coin if they start their turn with 3 or more coins.
     * When arrested, they pay 2 coins to the bank instead of transferring 1 to the attacker.
     */
    class Merchant : public Player {
        public:

            /**
         * @brief Constructs a Merchant and registers them to the game.
         * @param game Reference to the game instance.
         * @param name Name of the player.
         */
        Merchant(Game& game, const std::string& name);

            /**
         * @brief Called at the start of the Merchant's turn.
         * Grants 1 bonus coin if the Merchant has at least 3 coins.
         */
        void onStartTurn(); 

            /**
         * @brief Called when the Merchant is arrested.
         * Instead of giving 1 coin to the attacker, the Merchant loses 2 coins to the bank.
         * @throws std::runtime_error if the Merchant has less than 2 coins.
         */
        void onArrest(); 
    };
}