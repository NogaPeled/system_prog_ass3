#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include "../include/Merchant.hpp"
#include "../include/Spy.hpp"
#include <stdexcept>

namespace coup {

    void Game::addPlayer(Player* p) {
        players_list.push_back(p);
    }

    std::vector<std::string> Game::players() const {
        std::vector<std::string> names;
        for (Player* p : players_list)
        {
            if (p->isAlive())
            {
                names.push_back(p->getName());
            }
        }
        return names;
    }

    std::string Game::turn() const {
        if (players_list.empty()) throw std::runtime_error("No players in the game");
        if (!players_list[current_turn_index]->isAlive()) throw std::runtime_error("Current player has been eliminated - call nextTurn()");
        return players_list[current_turn_index]->getName();
    }

    void Game::nextTurn() {
        if (players_list.empty()) return;

        // Save the player who just finished their turn
        Player* justFinished = players_list[current_turn_index];

        // Advance to next alive player
        do {
            current_turn_index = (current_turn_index + 1) % players_list.size();
        } while (!players_list[current_turn_index]->isAlive());

        // Clear sanction for the player who just finished their turn
        justFinished->setUnderSanction(false);  // ✅ KEY LINE

        // New player's turn begins
        Player* nowPlaying = players_list[current_turn_index];
        nowPlaying->resetBribe();
        nowPlaying->setArrestDisabled(false);  // ✅ Reset spy block

        // Merchant bonus
        Merchant* merchant = dynamic_cast<Merchant*>(nowPlaying);
        if (merchant) {
            merchant->onStartTurn();
        }
    }

    std::string Game::winner() const {
        int aliveCount = 0;
        std::string lastAlive;
        for (Player* p : players_list)
        {
            if (p->isAlive())
            {
                aliveCount++;
                lastAlive = p->getName();
            }
            
        }
        
        if (aliveCount == 1) return lastAlive;
        throw std::runtime_error("Game is still ongoing");
    }

    const std::vector<Player*>& Game::getPlayers() const {
        return players_list;
    }
}

