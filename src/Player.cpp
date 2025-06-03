//nogapeled19@gmail.com
#include "../include/Player.hpp"
#include "../include/Game.hpp"
#include "../include/Baron.hpp"
#include "../include/General.hpp"
#include "../include/Judge.hpp"
#include "../include/Merchant.hpp"
#include "../include/Governor.hpp"
#include "../include/Spy.hpp"
#include <stdexcept>

namespace coup {

    Player::Player(Game& g, const std::string& name) : game(g), name(name) // Constructor: register the player in the game
    {
        game.addPlayer(this);
    }

    void Player::enforceCoupRule() const // Throws if player has 10 or more coins but hasn't played coup
    { 
        if (getCoins() >= 10) throw std::runtime_error(name + " has 10 or more coins and must perform a coup.");
    }

    void Player::gather() // Gather 1 coin from bank unless under sanction
    {
        enforceCoupRule();
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (underSanction) {
            throw std::runtime_error("You are under sanction and cannot gather.");
        }
        earnCoins(1);
    }

    void Player::tax() // Default tax: gain 2 coins (Governor overrides this)
    {
        enforceCoupRule();
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (underSanction) {
            throw std::runtime_error("You are under sanction and cannot tax.");
        }
        earnCoins(2);  // Default tax behavior
    }

    void Player::arrest(Player& target) // Attempt to arrest another player (steal 1 coin)
    {
        enforceCoupRule();

        // Check if any Spy wants to block this player's arrest
        for (Player* p : game.getPlayers()) {
            Spy* spy = dynamic_cast<Spy*>(p);
            if (spy && spy->isAlive()) {
                try {
                    spy->preventNextArrest(*this);  // Throws if this player is being blocked
                } catch (const std::exception& e) {
                    throw;  // Rethrow to be caught in GUI
                }
            }
        }
        // Can't arrest the same player as last turn
        if (game.getLastArrestedTarget() == &target) throw std::runtime_error(target.getName() + " was already arrested 🔒 last turn and cannot be arrested again until someone else is.");
        if (isArrestDisabled()) throw std::runtime_error("You are blocked from using arrest this turn (Spy effect).");
        if (target.isAlive() == false) throw std::runtime_error("Cannot arrest a dead player");
        if (target.getCoins() < 1) throw std::runtime_error("Target has no coins to steal");

        Merchant* merchant = dynamic_cast<Merchant*>(&target); // Merchant: custom logic for arrest
        if (merchant != nullptr) {
            merchant->onArrest();
        } else {
            target.loseCoins(1);
            earnCoins(1);

            General* general = dynamic_cast<General*>(&target); // General: refunds arrested coin
            if (general) general->onArrest();
        }

        game.setLastArrestedTarget(&target);
    }

    void Player::sanction(Player& target, int cost) // Sanction another player to block gather/tax
    { 
        enforceCoupRule();
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (getCoins() < cost) throw std::runtime_error("Not enough coins for sanction");
        if (!target.isAlive()) throw std::runtime_error("Cannot sanction a dead player");

        loseCoins(cost);

        // Apply role-based effects first
        if (Baron* baron = dynamic_cast<Baron*>(&target)) {
            baron->onSanction();
        }

        if (Judge* judge = dynamic_cast<Judge*>(&target)) {
            judge->onSanctionedBy(*this);  // attacker = this
        }

        // Only now apply the sanction
        target.setUnderSanction(true);
    }

    void Player::setUnderSanction(bool status) { // Set or unset sanction flag
        underSanction = status;
    }

    bool Player::isUnderSanction() const { // Check if under sanction
        return underSanction;
    }

    void Player::bribe() // Bribe to gain another action
    {
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (getCoins() < 4) throw std::runtime_error("Not enough coins to bribe");
        if (bribeUsed) throw std::runtime_error("You already used bribe this turn");

        loseCoins(4);
        bribeUsed = true;
        addAction();  // 🎯 GIVE AN EXTRA ACTION!
    }

    void Player::resetBribe() { // Reset bribe usage flag for new turn
        bribeUsed = false;
    }

    void Player::coup(Player& target) // Coup: eliminate another player for 7 coins
    {
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (!target.isAlive()) throw std::runtime_error("Target already eliminated.");
        if (getCoins() < 7) throw std::runtime_error("Not enough coins for coup.");

        loseCoins(7);
        target.eliminate();    
    }

    int Player::getCoins() const { // Return number of coins the player has
        return coins;
    }
    
    const std::string& Player::getName() const { // Get player name
        return name;
    }
        
    void Player::earnCoins(int amount) { // Add coins (validate non-negative)
        if (amount < 0) throw std::runtime_error("Cannot earn negative coins");
        coins += amount;
    }

    void Player::loseCoins(int amount) { // Subtract coins (validate enough funds)
        if (amount > coins) throw std::runtime_error("Not enough coins to lose");
        coins -= amount;
    }

    void Player::eliminate() { // Eliminate player from game
        alive = false;
    }

    bool Player::isAlive() const { // Is player still alive?
        return alive;
    }

    void Player::setArrestDisabled(bool b) { // Block/unblock arrest
        blockedFromArrest = b;
    }

    bool Player::isArrestDisabled() const { // Check if arrest is blocked
        return blockedFromArrest;
    }

    void Player::resetActions() { // Reset to 1 action at start of turn
    actionsLeft = 1;
    }

    void Player::addAction() { // Add an extra action (e.g. from bribe)
        actionsLeft++;
    }

    void Player::useAction() { // Consume one action
        if (actionsLeft > 0) actionsLeft--;
    }

    int Player::getActionsLeft() const { // Remaining actions this turn
        return actionsLeft;
    }
}