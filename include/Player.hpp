#pragma once
#include <string>

namespace coup {

class Game;

/**
 * @brief Base class for all player roles (Governor, Spy, Baron, etc.)
 * 
 * Contains shared properties and actions, and allows role-specific behavior
 * through virtual methods like tax().
 */
class Player {
protected:
    Game& game;               // Reference to the game instance
    std::string name;         // Player's name
    int coins = 0;            // Number of coins held
    bool alive = true;        // Whether the player is still active in the game
    bool underSanction = false;
    bool bribeUsed = false;
    bool blockedFromArrest = false;
    int actionsLeft = 1;  // Starts with 1 action by default

    // Throws if player has 10+ coins and tries to avoid coup
    void enforceCoupRule() const;

public:
    // Constructor and virtual destructor for polymorphic deletion
    Player(Game& game, const std::string& name);
    virtual ~Player() = default;

    // ===== Shared player actions =====
    void gather();                        // +1 coin, blocked by sanction
    virtual void tax();                   // Default: +2 coins; Governor overrides with +3
    void bribe();                         // Pay 4 coins to get a 2nd action
    void coup(Player& target);           // Eliminate a player for 7 coins
    void arrest(Player& target);         // Steal 1 coin unless blocked by Spy
    void sanction(Player& target, int cost); // Apply sanction

    // ===== Game state flags =====
    void setUnderSanction(bool status);
    bool isUnderSanction() const;

    void setArrestDisabled(bool);
    bool isArrestDisabled() const;

    bool hasUsedBribe() const { return bribeUsed; }
    void resetBribe();

    // ===== Coins and elimination =====
    void earnCoins(int amount);
    void loseCoins(int amount);
    int getCoins() const;

    void eliminate();          // Mark player as out of game
    bool isAlive() const;

    // ===== Identity =====
    const std::string& getName() const;

    // ===== Bribe-related turn tracking =====
    void resetActions();         // Called at start of turn: sets actionsLeft = 1
    void addAction();            // Called when bribe is used → actionsLeft++
    void useAction();            // Called when an action is performed → actionsLeft--
    int getActionsLeft() const;  // To check how many actions remain
    bool hasMoreActions() const { return actionsLeft > 1; }

};

} // namespace coup
