//nogapeled19@gmail.com
#pragma once
#include "Player.hpp"

namespace coup {

/**
 * @class Judge
 * @brief Represents the Judge role in the Coup game.
 *
 * The Judge can cancel a player's bribe and impose a penalty on anyone who sanctions them.
 */
class Judge : public Player {
    public:
    /**
     * @brief Constructs a Judge and registers them to the game.
     * @param game Reference to the game instance.
     * @param name Name of the player.
     */
    Judge(Game& game, const std::string& name);

    /**
     * @brief Cancels a bribe used by a player and removes their extra action for the turn.
     *
     * This method is called only if the Judge confirms the block via a popup prompt in the GUI.
     * @param target The player whose bribe is being undone.
     */
    void undoBribe(Player& target);

    /**
     * @brief Called when the Judge is sanctioned.
     * The attacker is penalized by losing 1 coin.
     * @param attacker The player who sanctioned the Judge.
     */
    void onSanctionedBy(Player& attacker);
};

}
