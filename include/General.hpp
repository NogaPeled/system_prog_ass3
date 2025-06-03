//nogapeled19@gmail.com
#pragma once
#include "Player.hpp"

namespace coup {

/**
 * @class General
 * @brief Represents the General role in the Coup game.
 *
 * The General can block a coup by paying 5 coins.
 * If arrested, the General regains 1 coin as compensation.
 */
class General : public Player {
    public:
    /**
     * @brief Constructs a General and registers them to the game.
     * @param game Reference to the game instance.
     * @param name Name of the player.
     */
    General(Game& game, const std::string& name);

    /**
     * @brief Blocks a coup targeting a player by paying 5 coins.
     *
     * This method is called only if the General confirms the block via a popup prompt in the GUI.
     * @throws std::runtime_error if the General doesn't have enough coins.
     */
    void blockCoup();

    /**
     * @brief Called when the General is arrested.
     * Restores 1 coin to the General.
     */
    void onArrest();
};

} 
