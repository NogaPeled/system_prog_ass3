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
        if (underSanction) throw std::runtime_error("You are under sanction and cannot gather.");
        earnCoins(1);
        game.nextTurn();
    }

    void Player::tax() {
        enforceCoupRule();
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (underSanction) throw std::runtime_error("You are under sanction and cannot tax.");

        Governor* gov = dynamic_cast<Governor*>(this);
        if (gov != nullptr) {
            earnCoins(3);
        } else {
            earnCoins(2);
        }

        game.nextTurn();
    }

    void Player::arrest(Player& target) {
        enforceCoupRule();
        if (isBlockedFromArrest()) throw std::runtime_error("You are blocked from using arrest this turn (Spy effect).");
        if(target.isBlockedFromArrest()) throw std::runtime_error(target.getName() + " is protected from arrest this turn (Spy)");
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (!target.isAlive()) throw std::runtime_error("Cannot arrest a dead player");
        if (&target == lastArrested) throw std::runtime_error("You cannot arrest the same player twice in a row");
        if (target.getCoins() < 1) throw std::runtime_error("Target has no coins to steal");

        Merchant* merchant = dynamic_cast<Merchant*>(&target);
        if (merchant != nullptr) // Returns a non-null pointer if target is a Merchant
        { 
            merchant->onArrest(); // Handles 2-coin loss
        }
        else
        {
            target.loseCoins(1);
            earnCoins(1);

            // Call target's onArrest() if it's a General
            General* general = dynamic_cast<General*>(&target);
            if (general != nullptr) // Returns a non-null pointer if target is a General
            {
                general->onArrest(); // Ensures the General gets the 1 coin refunded automatically
            }
        }
        
        lastArrested = &target;
        game.nextTurn();

    }

    void Player::sanction(Player& target) {
        enforceCoupRule();
        if (game.turn() != name) throw std::runtime_error("Not your turn!");
        if (getCoins() < 3) throw std::runtime_error("Not enough coins for sanction");
        if (!target.isAlive()) throw std::runtime_error("Cannot sanction a dead player");

        loseCoins(3);
        target.setUnderSanction(true);

        // If the target is a Baron, give them 1 coin
        Baron* baron = dynamic_cast<Baron*>(&target); // Returns a non-null pointer if target is a Baron
        if (baron != nullptr)
        {
            baron->onSanction();
        }
        
        Judge* judge = dynamic_cast<Judge*>(&target);
        if (judge != nullptr)
        {
            judge->onSanctionedBy(*this); // this = attacker
        }
        
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
            // Check if any Judge is alive and can cancel
        for (Player* p : game.getPlayers()) {
            if (p->isAlive() && dynamic_cast<Judge*>(p)) {
                // Judge blocks the bribe
                throw std::runtime_error("Bribe was blocked by a Judge! You lost 4 coins.");
            }
        }
        bribeUsed = true;
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

    void Player::setBlockedFromArrest(bool b) {
        blockedFromArrest = b;
    }

    bool Player::isBlockedFromArrest() const {
        return blockedFromArrest;
    }
}