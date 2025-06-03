//nogapeled19@gmail.com
#pragma once
#include "Player.hpp"

namespace coup {

        /**
     * @class Governor
     * @brief Represents the Governor role in the Coup game.
     *
     * The Governor gains 3 coins instead of 2 when using the tax action.
     * Can also block other players’ tax actions (handled in the GUI/game logic).
     */
    class Governor : public Player {
        public:

            /**
         * @brief Constructs a Governor and registers them to the game.
         * @param game Reference to the game instance.
         * @param name Name of the player.
         */
        Governor(Game& game, const std::string& name);

            /**
         * @brief Overrides the default tax action to give 3 coins.
         * @throws std::runtime_error if the player is under sanction or it's not their turn.
         */
        void tax() override;
    };
}