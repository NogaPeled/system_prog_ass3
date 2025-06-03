//nogapeled19@gmail.com
#pragma once
#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QString>
#include <vector>
#include <string>
#include "Game.hpp"
#include "Player.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"

/**
 * @class CoupGUI
 * @brief Manages the graphical interface and user interaction for the Coup game.
 *
 * This class handles displaying players, managing turns and actions,
 * and prompting interactive choices for role-based abilities such as
 * blocking tax (Governor), blocking coup (General), blocking bribe (Judge),
 * and blocking arrest (Spy) via popup dialogs.
 */
class CoupGUI : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs the GUI and initializes players, buttons, and layout.
     * @param parent Optional QWidget parent.
     */
    explicit CoupGUI(QWidget* parent = nullptr);

    /**
     * @brief Destructor that cleans up dynamically allocated players.
     */
    ~CoupGUI();

private slots:
    /** @brief Handles the Gather action (+1 coin). */
    void onGather();

    /** @brief Handles the Tax action (+2/3 coins). Governors may block via popup. */
    void onTax();

    /** @brief Handles the Coup action. Generals may block via popup. */
    void onCoup();

    /** @brief Handles the Sanction action (blocks gather/tax of the target). */
    void onSanction();

    /** @brief Handles the Arrest action. Spies may block via popup. */
    void onArrest();

    /** @brief Handles the Bribe action. Judges may block via popup. */
    void onBribe();

    /** @brief Handles the Spy action (view coins and block arrest). */
    void onSpy();

    /** @brief Handles the Baron’s Invest action (3→6 coins). */
    void onInvest();

    /** @brief Ends the current turn and passes to the next alive player. */
    void onNextTurn();

private:
    coup::Game game;                          /**< The Coup game logic instance */
    std::vector<coup::Player*> players;       /**< All players in the game */

    QListWidget* playerList;                  /**< Display list of players and coin counts */
    QTextEdit* logArea;                       /**< Log area for game events */
    QGridLayout* cardLayout;                  /**< Layout for showing player cards */
    std::vector<QWidget*> playerCards;        /**< Widgets representing each player */

    /**
     * @brief Updates the player list and card display after each action or turn.
     */
    void updatePlayerList();

    /**
     * @brief Creates a player card widget with emoji, name, and role description.
     * @param p The player to display.
     * @return Pointer to the generated QWidget.
     */
    QWidget* createPlayerCard(coup::Player* p);

    /**
     * @brief Opens a dialog to choose a target for an action.
     * @param currentName Name of the acting player.
     * @param actionTitle The title of the popup.
     * @return Pointer to the selected target player, or nullptr.
     */
    coup::Player* selectTarget(const std::string& currentName, const QString& actionTitle);

    /**
     * @brief Returns the role name of a given player.
     * @param p The player to check.
     * @return Role name as QString.
     */
    QString getRoleName(coup::Player* p);

    /**
     * @brief Returns the emoji associated with a given role.
     * @param role Role name.
     * @return Emoji as QString.
     */
    QString getRoleEmoji(const QString& role);

    /**
     * @brief Returns a brief description of a role's abilities.
     * @param role Role name.
     * @return Description string.
     */
    QString getRoleAbility(const QString& role);

    /**
     * @brief Displays an error popup and logs the message in the log area.
     * @param msg The error message to show.
     */
    void showError(const std::string& msg);

    /**
     * @brief Checks whether the game has a winner and handles endgame.
     */
    void checkForWinner();

    /**
     * @brief Disables all buttons (used when the game ends).
     */
    void disableAllButtons();
};
