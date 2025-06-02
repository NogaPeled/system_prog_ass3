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

    Player::Player(Game& g, const std::string& name) : game(g), name(name)
    {
        game.addPlayer(this);
    }

    void Player::enforceCoupRule() const {
        if (getCoins() >= 10) throw std::runtime_error(name + "has 10 or more coins and must perform a coup.");
    }

    void Player::gather() {
        enforceCoupRule();
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (underSanction) {
            throw std::runtime_error("You are under sanction and cannot gather.");
        }
        earnCoins(1);
        underSanction = false;
        game.nextTurn();
    }

    void Player::tax() {
        enforceCoupRule();
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (underSanction) {
            throw std::runtime_error("You are under sanction and cannot tax.");
        }
        Governor* gov = dynamic_cast<Governor*>(this);
        if (gov != nullptr) {
            earnCoins(3);
        } else {
            earnCoins(2);
        }

        underSanction = false; // only after successful tax
        game.nextTurn();
    }

    void Player::arrest(Player& target) {
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

        if (game.getLastArrestedTarget() == &target)
            throw std::runtime_error(target.getName() + " was already arrested 🔒 last turn and cannot be arrested again until someone else is.");
        if (isArrestDisabled()) throw std::runtime_error("You are blocked from using arrest this turn (Spy effect).");
        if (target.isAlive() == false) throw std::runtime_error("Cannot arrest a dead player");
        if (target.getCoins() < 1) throw std::runtime_error("Target has no coins to steal");

        Merchant* merchant = dynamic_cast<Merchant*>(&target);
        if (merchant != nullptr) {
            merchant->onArrest();
        } else {
            target.loseCoins(1);
            earnCoins(1);

            General* general = dynamic_cast<General*>(&target);
            if (general) general->onArrest();
        }

        game.setLastArrestedTarget(&target);
        game.nextTurn();
    }

    void Player::sanction(Player& target, int cost) {
        enforceCoupRule();
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (getCoins() < cost) throw std::runtime_error("Not enough coins for sanction");
        if (!target.isAlive()) throw std::runtime_error("Cannot sanction a dead player");

        loseCoins(cost);

        // Apply role bonuses/penalties first
        if (Baron* baron = dynamic_cast<Baron*>(&target)) {
            baron->onSanction();
        }

        if (Judge* judge = dynamic_cast<Judge*>(&target)) {
            judge->onSanctionedBy(*this);  // attacker = this
        }

        // Only now apply the sanction
        target.setUnderSanction(true);

        game.nextTurn();
    }

    void Player::setUnderSanction(bool status) {
        underSanction = status;
    }

    bool Player::isUnderSanction() const {
        return underSanction;
    }

    void Player::bribe() {
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (getCoins() < 4) throw std::runtime_error("Not enough coins to bribe");
        if (bribeUsed) throw std::runtime_error("You already used bribe this turn");
        
        loseCoins(4);
        // Judge popup logic is handled in GUI after bribe() is called
        bribeUsed = true;
        grantExtraTurn();  // Player gets 2 full actions after bribe
        // Do not call game.nextTurn() — player keeps the turn!
    }

    void Player::resetBribe() {
        bribeUsed = false;
    }

    int Player::getCoins() const {
        return coins;
    }
    
    const std::string& Player::getName() const {
        return name;
    }
        
    void Player::earnCoins(int amount) {
        if (amount < 0) throw std::runtime_error("Cannot earn negative coins");
        coins += amount;
    }

    void Player::loseCoins(int amount) {
        if (amount > coins) throw std::runtime_error("Not enough coins to lose");
        coins -= amount;
    }

    void Player::eliminate() {
        alive = false;
    }

    bool Player::isAlive() const {
        return alive;
    }

    void Player::setArrestDisabled(bool b) {
        blockedFromArrest = b;
    }

    bool Player::isArrestDisabled() const {
        return blockedFromArrest;
    }

}