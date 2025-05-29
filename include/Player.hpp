#pragma once
#include <string>
// The base class for all player roles.

// Designing an abstract base class called Player, which other classes like Governor, Spy, and Baron will inherit from
namespace coup {
    class Game;

    class Player {

        protected: 
        Game& game; // Reference to the game this player belongs to
        std::string name;
        int coins = 0; // Number of coins the player currently holds
        bool alive = true; // Whether the player is still in the game
        Player* lastArrested = nullptr;
        bool underSanction = false;
        bool bribeUsed = false; // to track if the player has used a bribe this turn
        void enforceCoupRule() const;
        bool blockedFromArrest = false;
        // These are `protected` so that derived classes (like `Governor`) can access them directly if needed.

        public:
        Player(Game& game, const std::string& name); // The constructor takes a Game& and std::string name.
        virtual ~Player() = default; // The destructor is virtual so derived classes can clean up correctly if needed.
        // ~Player() = default; because I don’t need custom cleanup yet.

        virtual void gather();
        virtual void tax();
        virtual void coup(Player& target) = 0;
        virtual void undo(Player& target) = 0;
        virtual void arrest(Player& target);
        virtual void sanction(Player& target);        
        void setUnderSanction(bool status);
        bool isUnderSanction() const;    
        virtual void bribe();    
        void resetBribe();

        int getCoins() const;
        const std::string& getName() const;

        void earnCoins(int amount);
        void loseCoins(int amount);
        void eliminate();
        bool isAlive() const;
        void setBlockedFromArrest(bool);
        bool isBlockedFromArrest() const;

    };
}
