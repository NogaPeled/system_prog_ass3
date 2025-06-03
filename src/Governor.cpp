//nogapeled19@gmail.com
#include "../include/Governor.hpp"
#include "../include/Game.hpp"
#include <stdexcept>

namespace coup {
    Governor::Governor(Game& game, const std::string& name) : Player(game, name) {} // Constructor: initialize a Governor player
    
    void Governor::tax() // Governor gains 3 coins instead of 2 when using tax
    {
        enforceCoupRule(); // Must coup if holding 10+ coins
        if (game.turn() != getName()) throw std::runtime_error("Not your turn!");
        if (isUnderSanction()) throw std::runtime_error("You are under sanction and cannot tax.");

        earnCoins(3);
        game.nextTurn(); // Ends the turn after using tax
    }
}