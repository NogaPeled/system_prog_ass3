//nogapeled19@gmail.com
#pragma once
#include <vector>
#include <string>

namespace coup {
    class Player;

        /**
     * @class Game
     * @brief Manages the game state, turn order, and player actions.
     *
     * Holds the list of players, tracks whose turn it is, and handles global game logic
     * such as advancing turns, determining the winner, and storing arrest-related state.
     */
    class Game {
        private:

        std::vector<Player*> players_list; // List of all players in the game
        int current_turn_index = 0; // Index of the player whose turn it is
        Player* lastArrestedTarget = nullptr; // Pointer to the player last arrested

        public:

        /** @brief Adds a new player to the game. */
        void addPlayer(Player* p);

        /** @brief Returns a list of currently alive players' names. */
        std::vector<std::string> players() const;

        /** @brief Returns the name of the current player whose turn it is. */
        std::string turn() const;

        /** @brief Advances the turn to the next alive player. */
        void nextTurn();

            /**
         * @brief Returns the winner's name if only one player remains.
         * @throws std::runtime_error if the game is still ongoing.
         */
        std::string winner() const;

        /** @brief Returns a reference to the full player list (including eliminated ones). */
        const std::vector<Player*>& getPlayers() const;

         /** @brief Sets the last player that was arrested. */
        void setLastArrestedTarget(Player* p);

        /** @brief Gets the last player that was arrested. */
        Player* getLastArrestedTarget() const;
    };
}