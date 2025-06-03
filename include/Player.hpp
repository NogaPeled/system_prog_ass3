//nogapeled19@gmail.com
/**
 * @file Player.hpp
 * @brief Defines the Player base class and its interface.
 *
 * This file contains the declaration of the Player class used in the Coup game.
 * The Player class encapsulates common attributes and actions that all player roles share.
 * Each role (Governor, Spy, Baron, etc.) inherits from this base and may override certain behaviors.
 */

#pragma once
#include <string>

namespace coup {

class Game;

/**
 * @class Player
 * @brief Base class for all player roles (Governor, Spy, Baron, General, Merchant, Judge)
 *
 * Contains shared properties and actions, and allows role-specific behavior
 * through virtual methods like tax(). Handles player state (coins, alive status),
 * interactions with other players (coup, arrest, sanction), and turn-related logic.
 */
class Player {
protected:
    Game& game;               // Reference to the game instance
    std::string name;         // Player's name
    int coins = 0;            // Number of coins held
    bool alive = true;        // Whether the player is still active in the game
    bool underSanction = false; // Flag indicating if player is under sanction
    bool bribeUsed = false;     // Flag for bribe usage this turn
    bool blockedFromArrest = false; // Flag set if blocked by Spy
    int actionsLeft = 1;         // Actions left in current turn (1 by default)

    /**
     * @brief Ensures a player with 10 or more coins must perform a coup.
     * @throws std::runtime_error if rule is violated.
     */
    void enforceCoupRule() const;

public:
    /**
     * @brief Constructs a player and registers them to the game.
     * @param game Reference to the game instance.
     * @param name Name of the player.
     */
    Player(Game& game, const std::string& name);

    virtual ~Player() = default;

    // ===== Shared player actions =====

    /** @brief Take 1 coin from the bank. Blocked if under sanction. */
    void gather();             
    
    /** @brief Take 2 coins from the bank (Governor overrides this to 3). */
    virtual void tax();    

    /**
     * @brief Pay 4 coins to gain an extra action this turn.
     * @throws std::runtime_error if already bribed or not enough coins.
     */        
    void bribe();            
    
    /**
     * @brief Pay 7 coins to eliminate another player.
     * @param target The player to eliminate.
     * @throws std::runtime_error if target is already dead or not enough coins.
     */
    void coup(Player& target);   
    
    /**
     * @brief Steal 1 coin from a player unless blocked.
     * @param target The player being arrested.
     */
    void arrest(Player& target);     
    
    /**
     * @brief Apply a sanction to prevent target from gathering/taxing.
     * @param target The player to sanction.
     * @param cost The number of coins to pay (3 or 4 if Judge involved).
     */
    void sanction(Player& target, int cost); 

    // ===== Game state flags =====

    /** @brief Set whether the player is currently under sanction. */
    void setUnderSanction(bool status);

    /** @brief Check if the player is under sanction. */
    bool isUnderSanction() const;

    /** @brief Disable player's ability to arrest for current turn. */
    void setArrestDisabled(bool);

    /** @brief Check if player is blocked from using arrest. */
    bool isArrestDisabled() const;

    /** @brief Check if the player has used bribe this turn. */
    bool hasUsedBribe() const { return bribeUsed; }

    /** @brief Reset bribe flag at the start of a new turn. */
    void resetBribe();

    // ===== Coins and elimination =====

    /** @brief Add coins to the player. */
    void earnCoins(int amount);

    /** @brief Subtract coins from the player. */
    void loseCoins(int amount);

    /** @brief Get current number of coins the player holds. */
    int getCoins() const;

    /** @brief Eliminate the player from the game. */
    void eliminate();

    /** @brief Check if the player is still active in the game. */
    bool isAlive() const;

    // ===== Identity =====

    /** @brief Get the player's name. */
    const std::string& getName() const;

    // ===== Bribe-related turn tracking =====

    /** @brief Reset action count to 1 at the start of the turn. */
    void resetActions();

    /** @brief Add an extra action (e.g., after bribe). */
    void addAction();

    /** @brief Use one available action. */
    void useAction();

    /** @brief Get number of actions left this turn. */
    int getActionsLeft() const;

    /** @brief Check if player has more than 1 action left. */
    bool hasMoreActions() const { return actionsLeft > 1; }
};

} // namespace coup
