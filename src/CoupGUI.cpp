//nogapeled19@gmail.com
#include "CoupGUI.hpp"
#include "../include/Player.hpp"
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QRandomGenerator>
#include <QTimer>


CoupGUI::CoupGUI(QWidget* parent) : QWidget(parent) { // Set window title and size
    setWindowTitle("Coup Game - GUI Version");
    resize(1000, 600);
    // Create main layouts: player list/log + card view + action buttons
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout();
    playerList = new QListWidget();
    logArea = new QTextEdit();
    logArea->setReadOnly(true);

    QWidget* cardContainer = new QWidget();
    cardLayout = new QGridLayout(cardContainer);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(playerList);
    leftLayout->addWidget(logArea);
    topLayout->addLayout(leftLayout, 1);
    topLayout->addWidget(cardContainer, 2);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* gatherBtn = new QPushButton("Gather");
    QPushButton* taxBtn = new QPushButton("Tax");
    QPushButton* coupBtn = new QPushButton("Coup");
    QPushButton* sanctionBtn = new QPushButton("Sanction");
    QPushButton* arrestBtn = new QPushButton("Arrest");
    QPushButton* bribeBtn = new QPushButton("Bribe");
    QPushButton* spyBtn = new QPushButton("Spy");
    QPushButton* investBtn = new QPushButton("Invest");
    QPushButton* nextBtn = new QPushButton("Next Turn");

    buttonLayout->addWidget(gatherBtn);
    buttonLayout->addWidget(taxBtn);
    buttonLayout->addWidget(coupBtn);
    buttonLayout->addWidget(sanctionBtn);
    buttonLayout->addWidget(arrestBtn);
    buttonLayout->addWidget(bribeBtn);
    buttonLayout->addWidget(spyBtn);
    buttonLayout->addWidget(investBtn);
    buttonLayout->addWidget(nextBtn);

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(buttonLayout);

    bool ok;
    int numPlayers = QInputDialog::getInt(this, "Players", "Enter number of players (2-6):", 2, 2, 6, 1, &ok); // Ask user how many players will participate
    if (!ok) QApplication::quit();

    QStringList rolePool = {"Governor", "Spy", "Baron", "General", "Judge", "Merchant"};

    for (int i = 0; i < numPlayers; ++i) {
        QString name = QInputDialog::getText(this, "Player Name", QString("Enter name for player %1:").arg(i + 1), QLineEdit::Normal, "", &ok); // Get each player's name and assign a random role
        if (!ok || name.isEmpty()) QApplication::quit();

        QString role = rolePool[QRandomGenerator::global()->bounded(rolePool.size())]; // Instantiate a player object based on randomly selected role

        coup::Player* newPlayer = nullptr;
        if (role == "Governor") newPlayer = new coup::Governor(game, name.toStdString());
        else if (role == "Spy") newPlayer = new coup::Spy(game, name.toStdString());
        else if (role == "Baron") newPlayer = new coup::Baron(game, name.toStdString());
        else if (role == "General") newPlayer = new coup::General(game, name.toStdString());
        else if (role == "Judge") newPlayer = new coup::Judge(game, name.toStdString());
        else if (role == "Merchant") newPlayer = new coup::Merchant(game, name.toStdString());

        players.push_back(newPlayer);
    }

    // Connect button to corresponding action handler
    connect(gatherBtn, &QPushButton::clicked, this, &CoupGUI::onGather);
    connect(taxBtn, &QPushButton::clicked, this, &CoupGUI::onTax);
    connect(coupBtn, &QPushButton::clicked, this, &CoupGUI::onCoup);
    connect(sanctionBtn, &QPushButton::clicked, this, &CoupGUI::onSanction);
    connect(arrestBtn, &QPushButton::clicked, this, &CoupGUI::onArrest);
    connect(bribeBtn, &QPushButton::clicked, this, &CoupGUI::onBribe);
    connect(spyBtn, &QPushButton::clicked, this, &CoupGUI::onSpy);
    connect(investBtn, &QPushButton::clicked, this, &CoupGUI::onInvest);
    connect(nextBtn, &QPushButton::clicked, this, &CoupGUI::onNextTurn);

    updatePlayerList();
}

void CoupGUI::onGather() { // Player performs gather unless under sanction
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current) {
            try {
                // Check if under sanction before calling gather
                if (p->isUnderSanction()) {
                    showError("You are under sanction and cannot gather.");
                    return;
                }

                p->gather();
                p->useAction();
                if (p->getActionsLeft() == 0) QTimer::singleShot(0, this, &CoupGUI::onNextTurn);
                logArea->append(QString::fromStdString(p->getName() + " gathered 1 coin."));
            } catch (const std::exception& e) {
                showError(e.what());
            }
            updatePlayerList();
            return;
        }
    }
}

void CoupGUI::onTax() { // Governors may optionally block the tax action via popup
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current) {
            try {
                // Check if under sanction before Governor interaction
                if (p->isUnderSanction()) {
                    showError("You are under sanction and cannot tax.");
                    return;
                }

                // Governor block opportunity only if not under sanction
                for (auto* gov : players) {
                    if (getRoleName(gov) == "Governor" &&
                        gov->isAlive() &&
                        gov->getName() != current) {

                        int res = QMessageBox::question(this, "Governor Tax Block",
                            QString::fromStdString(gov->getName()) +
                            ": Do you want to block the Tax action of " +
                            QString::fromStdString(current) + "?",
                            QMessageBox::Yes | QMessageBox::No);

                        if (res == QMessageBox::Yes) {
                            logArea->append(QString::fromStdString("🏛️ " + gov->getName() +
                                " blocked the Tax action of " + current));
                            updatePlayerList();
                            checkForWinner();  // check if only one player remains
                            return;
                        }
                    }
                }

                // No one blocked → proceed with tax
                p->tax();
                p->useAction();
                if (p->getActionsLeft() == 0) QTimer::singleShot(0, this, &CoupGUI::onNextTurn);
                logArea->append(QString::fromStdString("🏛️ " + p->getName() + " used tax."));
            } catch (const std::exception& e) {
                showError(e.what());
                logArea->append(QString::fromStdString("🚫 " + p->getName() + " couldn't use tax."));
            }
            updatePlayerList();
            return;
        }
    }
}

void CoupGUI::onCoup() { // General may choose to block the coup via popup
    auto current = game.turn();
    coup::Player* actor = nullptr;
    for (auto* p : players) {
        if (p->getName() == current) {
            actor = p;
            break;
        }
    }
    if (!actor) return;

    // Check if attacker has enough coins before proceeding
    if (actor->getCoins() < 7) {
        showError("You need at least 7 coins to perform a coup.");
        return;
    }

    coup::Player* target = selectTarget(current, "Coup");
    if (!target) return;

    // Let each General choose whether to block the coup
    for (auto* general : players) {
        if (general == actor) continue;  // Skip self-blocking

        if (getRoleName(general) == "General" &&
            general->isAlive() &&
            general->getCoins() >= 5) {

            int res = QMessageBox::question(this, "General Coup Block",
                QString::fromStdString(general->getName()) + ": Do you want to pay 5 coins to block the coup on " +
                QString::fromStdString(target->getName()) + "?",
                QMessageBox::Yes | QMessageBox::No);

            if (res == QMessageBox::Yes) {
                try {
                    actor->loseCoins(7);       // attacker always pays
                    general->loseCoins(5);     // general pays to block

                    logArea->append(QString::fromStdString("🛡️ " + general->getName() +
                        " blocked the coup on " + target->getName()));

                    actor->useAction();  // Consume action
                    if (actor->getActionsLeft() == 0)
                        QTimer::singleShot(0, this, &CoupGUI::onNextTurn);  // Advance turn

                    updatePlayerList();
                    checkForWinner();
                    return;
                } catch (const std::exception& e) {
                    showError(e.what());
                    updatePlayerList();
                    return;
                }
            }
        }
    }

    // No General blocked → proceed with coup
    try {
        actor->coup(*target);
        actor->useAction();
        if (actor->getActionsLeft() == 0)
            QTimer::singleShot(0, this, &CoupGUI::onNextTurn);
        logArea->append(QString::fromStdString("💥 " + current + " performed a coup on " + target->getName()));
        updatePlayerList();
        checkForWinner();
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void CoupGUI::onSanction() { // GUI pre-check for cost; sanction logic handled in backend
    auto current = game.turn();
    coup::Player* actor = nullptr;
    for (auto* p : players) {
        if (p->getName() == current) {
            actor = p;
            break;
        }
    }
    if (!actor) return;
    auto* target = selectTarget(current, "Sanction");
    if (!target) return;

    try {
        int cost = 3;
        logArea->append(QString::fromStdString("🧮 " + actor->getName() + " has " + std::to_string(actor->getCoins()) + " coins. Sanction cost: " + std::to_string(cost)));
        //  GUI-level check before calling backend logic
        if (actor->getCoins() < cost) {
            throw std::runtime_error(std::string("Not enough coins for sanction") + (cost == 4 ? " (Judge requires 4 coins)" : ""));
        }

        // Perform the sanction with cost passed in
        actor->sanction(*target, cost);
        actor->useAction();  // Consume an action!
        if (actor->getActionsLeft() == 0) QTimer::singleShot(0, this, &CoupGUI::onNextTurn);
        logArea->append(QString::fromStdString("🚫 " + current + " sanctioned " + target->getName()));
        updatePlayerList();

    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void CoupGUI::onArrest() {// Spies may choose to block arrest via popup
    auto current = game.turn();
    coup::Player* actor = nullptr;
    for (auto* p : players) {
        if (p->getName() == current) {
            actor = p;
            break;
        }
    }
    if (!actor) return;
    coup::Player* target = selectTarget(current, "Arrest");
    if (!target) return;

    // Check if any Spy wants to block the arrest
    for (auto* spy : players) {
        if (getRoleName(spy) == "Spy" && spy->isAlive()) {
            coup::Spy* realSpy = dynamic_cast<coup::Spy*>(spy);
            if (realSpy && realSpy->getBlockedTarget() == actor) {
                int res = QMessageBox::question(this, "Spy Arrest Block",
                    QString::fromStdString(spy->getName()) + ": Do you want to block " +
                    QString::fromStdString(actor->getName()) + "'s arrest action?",
                    QMessageBox::Yes | QMessageBox::No);

                if (res == QMessageBox::Yes) {
                    actor->setArrestDisabled(true); // prevent the arrest
                    realSpy->clearWatch(); // reset watch after using
                    logArea->append(QString::fromStdString("🕵️ " + spy->getName() +
                        " blocked " + actor->getName() + "'s arrest."));
                    return;
                } else {
                    logArea->append(QString::fromStdString("🕵️ " + spy->getName() +
                        " chose not to block " + actor->getName() + "'s arrest."));
                    realSpy->clearWatch();  // also clear even if not used
                }
            }
        }
    }

    // Proceed with arrest if not blocked
    try {
        actor->arrest(*target);
        actor->useAction();  // Consume an action!
        if (actor->getActionsLeft() == 0) QTimer::singleShot(0, this, &CoupGUI::onNextTurn);
        logArea->append(QString::fromStdString(current + " arrested " + target->getName()));
        updatePlayerList();
    } catch (const std::exception& e) {
        showError(e.what());
    }
}


void CoupGUI::onBribe() { // Judges may choose to block bribe via popup
    auto current = game.turn();
    coup::Player* actor = nullptr;
    for (auto* p : players)
        if (p->getName() == current) {
            actor = p;
            break;
        }
    if (!actor) return;

    try {
        actor->bribe();  // deducts 4 coins and marks bribeUsed = true
        logArea->append(QString::fromStdString("💸 " + current + " used a bribe."));

        // Ask all judges if they want to block it
        for (auto* judge : players) {
            if (getRoleName(judge) == "Judge" && judge->isAlive() && judge->getName() != current) {
                int res = QMessageBox::question(this, "Judge Bribe Block",
                    QString::fromStdString(judge->getName()) + ": Do you want to block " +
                    QString::fromStdString(current) + "'s bribe?",
                    QMessageBox::Yes | QMessageBox::No);

                if (res == QMessageBox::Yes) {
                    logArea->append(QString::fromStdString("⚖️ " + judge->getName() +
                        " blocked the bribe! " + current + " lost 4 coins."));
                    dynamic_cast<coup::Judge*>(judge)->undoBribe(*actor);  // ❗ cancel the extra action
                    game.nextTurn();               //  advance turn!
                    updatePlayerList();
                    checkForWinner();  // check if only one player remains
                    return;
                }
            }
        }

        // Bribe succeeded → player still on same turn
        logArea->append(QString::fromStdString("🔁 " + current + " may take another action this turn."));
        updatePlayerList();

    } catch (const std::exception& e) {
        showError(e.what());
    }
}


void CoupGUI::onSpy() { // Spy sees coin count and tracks for arrest block
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current && getRoleName(p) == "Spy") {
            auto* target = selectTarget(current, "Spy");
            if (!target) return;

            QString msg = QString::fromStdString(target->getName()) + " has " + QString::number(target->getCoins()) + " coins.";
            QMessageBox::information(this, "Spy Result", msg);

            // Track the target in Spy
            coup::Spy* spy = dynamic_cast<coup::Spy*>(p);
            if (spy) spy->watch(*target);

            logArea->append(QString::fromStdString(current + " spied on " + target->getName()));
            return;
        }
    }
    showError("Only a Spy can use this action.");
}



void CoupGUI::onInvest() { // Baron invests 3 to gain 6 coins
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current && getRoleName(p) == "Baron") {
            try {
                dynamic_cast<coup::Baron*>(p)->invest();
                p->useAction();  // Consume an action!
                if (p->getActionsLeft() == 0) QTimer::singleShot(0, this, &CoupGUI::onNextTurn);
                logArea->append(QString::fromStdString(current + " invested and gained 6 coins."));
                updatePlayerList();
            } catch (const std::exception& e) {
                showError(e.what());
            }
            return;
        }
    }
    showError("Only a Baron can use Invest.");
}

void CoupGUI::onNextTurn() { // Optionally skip remaining actions; resets spy and turn state
    try {
        // Find current player
        coup::Player* currentPlayer = nullptr;
        for (auto* p : players) {
            if (p->getName() == game.turn()) {
                currentPlayer = p;
                break;
            }
        }
        if (!currentPlayer) return;

        // If player has more actions left, DO NOT advance the turn
        if (currentPlayer->getActionsLeft() > 0) {
            int res = QMessageBox::question(this, "Skip Turn",
                "You still have actions left. Do you want to skip them?",
                QMessageBox::Yes | QMessageBox::No);
            if (res == QMessageBox::No) return;
        }

        // 🕵️ Clear Spy’s watch on the just-finished player
        for (auto* spy : players) {
            if (getRoleName(spy) == "Spy") {
                coup::Spy* realSpy = dynamic_cast<coup::Spy*>(spy);
                if (realSpy && realSpy->getBlockedTarget() == currentPlayer) {
                    realSpy->clearWatch();
                }
            }
        }

        // Advance to next player
        game.nextTurn();

        // Reset new player's state
        for (auto* p : players) {
            if (p->getName() == game.turn()) {
                p->resetActions();  // Start with 1 action
                p->resetBribe();    // Allow bribe again
                break;
            }
        }

        updatePlayerList();
        checkForWinner();

    } catch (const std::exception& e) {
        showError(e.what());
    }
}

QWidget* CoupGUI::createPlayerCard(coup::Player* p) { // Build visual card for a player's info, role, and abilities
    QString role = getRoleName(p);
    QWidget* card = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(card);

    QLabel* emoji = new QLabel(getRoleEmoji(role));
    emoji->setAlignment(Qt::AlignCenter);
    emoji->setStyleSheet("font-size: 28px;");
    emoji->setFont(QFont("Segoe UI Emoji"));  // Ensure emoji displays

    QLabel* name = new QLabel(QString::fromStdString(p->getName()) + " (" + role + ")");
    name->setAlignment(Qt::AlignCenter);
    QLabel* desc = new QLabel(getRoleAbility(role));
    desc->setAlignment(Qt::AlignCenter);
    desc->setWordWrap(true);

    layout->addWidget(emoji);
    layout->addWidget(name);
    layout->addWidget(desc);

    return card;
}

coup::Player* CoupGUI::selectTarget(const std::string& currentName, const QString& actionTitle) { // Let the current player choose a valid living target
    QStringList choices;
    for (auto* p : players) {
        if (p->isAlive() && p->getName() != currentName)
            choices << QString::fromStdString(p->getName());
    }
    bool ok;
    QString choice = QInputDialog::getItem(this, actionTitle, "Choose a player:", choices, 0, false, &ok);
    if (!ok || choice.isEmpty()) return nullptr;
    for (auto* p : players) {
        if (QString::fromStdString(p->getName()) == choice) return p;
    }
    return nullptr;
}

QString CoupGUI::getRoleName(coup::Player* p) {
    if (dynamic_cast<coup::Governor*>(p)) return "Governor";
    if (dynamic_cast<coup::Spy*>(p)) return "Spy";
    if (dynamic_cast<coup::Baron*>(p)) return "Baron";
    if (dynamic_cast<coup::General*>(p)) return "General";
    if (dynamic_cast<coup::Judge*>(p)) return "Judge";
    if (dynamic_cast<coup::Merchant*>(p)) return "Merchant";
    return "Unknown";
}

QString CoupGUI::getRoleEmoji(const QString& role) {
    static std::map<QString, QString> emojis = {
        {"Governor", "🏛️"}, {"Spy", "🕵️"}, {"Baron", "💰"},
        {"General", "🛡️"}, {"Judge", "⚖️"}, {"Merchant", "📦"}
    };
    return emojis[role];
}

QString CoupGUI::getRoleAbility(const QString& role) {
    static std::map<QString, QString> descs = {
        {"Governor", "Tax = 3 coins, can block other's tax"},
        {"Spy", "See coins of others, can block arrest"},
        {"Baron", "Invest 3 to get 6. Gains 1 if sanctioned"},
        {"General", "Pay 5 to block coup on self/others"},
        {"Judge", "Can block bribe. Sanction costs +1"},
        {"Merchant", "Gets extra coin if starts turn with 3+"}
    };
    return descs[role];
}

void CoupGUI::showError(const std::string& msg) {
    logArea->append("❌ " + QString::fromStdString(msg));  
    QMessageBox::warning(this, "Error", QString::fromStdString(msg));
}


CoupGUI::~CoupGUI() {
    for (auto* p : players) delete p;
}

void CoupGUI::checkForWinner() {
    try {
        QString winner = QString::fromStdString(game.winner());
        logArea->append("🏆 The winner is: " + winner + "!");
        QMessageBox::information(this, "Game Over", "🏆 The winner is: " + winner + "!");
        disableAllButtons();
    } catch (const std::exception&) {
        // Game is still ongoing — do nothing
    }
}

void CoupGUI::disableAllButtons() {
    for (QPushButton* btn : findChildren<QPushButton*>()) {
        btn->setEnabled(false);
    }
}

void CoupGUI::updatePlayerList() { // Refreshes the GUI: updates list, coin count, turn indicator, and role cards
    logArea->append("🟢 It's " + QString::fromStdString(game.turn()) + "'s turn.");
    playerList->clear();
    for (QWidget* w : playerCards) delete w;
    playerCards.clear();

    for (auto* p : players) {
        QString display = QString::fromStdString(p->getName());
        if (!p->isAlive()) {
            display = "❌ " + display;
        } else if (p->getName() == game.turn()) {
            display += " ← (TURN)";
        }
        display += ": " + QString::number(p->getCoins()) + " coins";
        playerList->addItem(display);

        QWidget* card = createPlayerCard(p);
        cardLayout->addWidget(card, playerCards.size() / 2, playerCards.size() % 2);
        playerCards.push_back(card);
    }
}

