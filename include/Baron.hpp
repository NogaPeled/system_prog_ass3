//nogapeled19@gmail.com
#pragma once
#include "Player.hpp"

namespace coup {

        /**
     * @class Baron
     * @brief Represents the Baron role in the Coup game.
     *
     * The Baron can invest 3 coins to gain 6 (a profit action).
     * When sanctioned, they are compensated with 1 coin automatically.
     */
    class Baron : public Player {
        public:

            /**
         * @brief Constructs a Baron player and registers them to the game.
         * @param game Reference to the game instance.
         * @param name Name of the player.
         */
        Baron(Game& game, const std::string& name);
        
                /**
         * @brief Invest 3 coins to gain 6 coins.
         * @throws std::runtime_error if the player does not have enough coins.
         */
        void invest();

            /**
         * @brief Called when the Baron is sanctioned.
         * Grants the Baron 1 coin as compensation.
         */
        void onSanction(); // special reaction when sanctioned
    };
}