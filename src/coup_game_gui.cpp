#include "CoupGUI.hpp"
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <map>
#include <random>
#include <algorithm>
#include <QApplication>
#include <QTime>

CoupGUI::CoupGUI(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Coup Game - GUI Version");
    resize(1000, 600);

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
    int numPlayers = QInputDialog::getInt(this, "Players", "Enter number of players (2-6):", 2, 2, 6, 1, &ok);
    if (!ok) QApplication::quit();

    QStringList rolePool = {"Governor", "Spy", "Baron", "General", "Judge", "Merchant"};
    qsrand(QTime::currentTime().msec());

    for (int i = 0; i < numPlayers; ++i) {
        QString name = QInputDialog::getText(this, "Player Name", QString("Enter name for player %1:").arg(i + 1), QLineEdit::Normal, "", &ok);
        if (!ok || name.isEmpty()) QApplication::quit();

        QString role = rolePool[qrand() % rolePool.size()];  // 🎲 pick random, allows duplicates

        coup::Player* newPlayer = nullptr;
        if (role == "Governor") newPlayer = new coup::Governor(game, name.toStdString());
        else if (role == "Spy") newPlayer = new coup::Spy(game, name.toStdString());
        else if (role == "Baron") newPlayer = new coup::Baron(game, name.toStdString());
        else if (role == "General") newPlayer = new coup::General(game, name.toStdString());
        else if (role == "Judge") newPlayer = new coup::Judge(game, name.toStdString());
        else if (role == "Merchant") newPlayer = new coup::Merchant(game, name.toStdString());

        players.push_back(newPlayer);
    }

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

void CoupGUI::onGather() {
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current) {
            try {
                p->gather();
                logArea->append(QString::fromStdString(p->getName() + " gathered 1 coin."));
                updatePlayerList();
            } catch (const std::exception& e) {
                showError(e.what());
                updatePlayerList();  // ✅ Refresh list even after error
            }
            return;
        }
    }
}

void CoupGUI::onTax() {
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current) {
            try {
                // Governor block opportunity
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
                            game.nextTurn();
                            updatePlayerList();
                            return;
                        }
                    }
                }

                // No one blocked
                p->tax();
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


void CoupGUI::onCoup() {
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
        if (getRoleName(general) == "General" && general->isAlive() && general->getCoins() >= 5) {
            int res = QMessageBox::question(this, "General Coup Block",
                QString::fromStdString(general->getName()) + ": Do you want to pay 5 coins to block the coup on " +
                QString::fromStdString(target->getName()) + "?",
                QMessageBox::Yes | QMessageBox::No);

            if (res == QMessageBox::Yes) {
                try {
                    actor->loseCoins(7);       // attacker always pays
                    general->loseCoins(5);     // general pays to block
                    logArea->append(QString::fromStdString("🛡️ " + general->getName() + " blocked the coup on " + target->getName()));
                    game.nextTurn();
                    updatePlayerList();
                    return;  // Coup blocked
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
        logArea->append(QString::fromStdString("💥 " + current + " performed a coup on " + target->getName()));
        updatePlayerList();
    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void CoupGUI::onSanction() {
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
        logArea->append(QString::fromStdString("🚫 " + current + " sanctioned " + target->getName()));
        updatePlayerList();

    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void CoupGUI::onArrest() {
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

    // ✅ Check if any Spy wants to block the arrest (Spy previously spied on the actor)
    for (auto* spy : players) {
        if (getRoleName(spy) == "Spy" && spy->isAlive()) {
            coup::Spy* realSpy = dynamic_cast<coup::Spy*>(spy);
            if (realSpy && realSpy->getBlockedTarget() == actor) {
                int res = QMessageBox::question(this, "Spy Arrest Block",
                    QString::fromStdString(spy->getName()) + ": Do you want to block " +
                    QString::fromStdString(actor->getName()) + "'s arrest action?",
                    QMessageBox::Yes | QMessageBox::No);
                if (res == QMessageBox::Yes) {
                    actor->setArrestDisabled(true);  // ✅ Prevent further arrest attempts this turn
                    logArea->append(QString::fromStdString("🕵️ " + spy->getName() +
                        " blocked " + actor->getName() + "'s arrest."));
                    return; // Arrest blocked
                }
            }
        }
    }

    // 🛑 Try to do the arrest if no spy blocked
    try {
        actor->arrest(*target);
        logArea->append(QString::fromStdString(current + " arrested " + target->getName()));
        updatePlayerList();
    } catch (const std::exception& e) {
        showError(e.what());
    }
}


void CoupGUI::onBribe() {
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

                    game.nextTurn();               //  advance turn!
                    updatePlayerList();
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


void CoupGUI::onSpy() {
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current && getRoleName(p) == "Spy") {
            auto* target = selectTarget(current, "Spy");
            if (!target) return;

            QString msg = QString::fromStdString(target->getName()) + " has " + QString::number(target->getCoins()) + " coins.";
            QMessageBox::information(this, "Spy Result", msg);

            // ✅ Track the target in Spy
            coup::Spy* spy = dynamic_cast<coup::Spy*>(p);
            if (spy) spy->watch(*target);

            logArea->append(QString::fromStdString(current + " spied on " + target->getName()));
            return;
        }
    }
    showError("Only a Spy can use this action.");
}



void CoupGUI::onInvest() {
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current && getRoleName(p) == "Baron") {
            try {
                dynamic_cast<coup::Baron*>(p)->invest();
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

void CoupGUI::onNextTurn() {
    try {
        for (auto* p : players) {
            if (getRoleName(p) == "Merchant" && p->getName() == game.turn() && p->getCoins() >= 3) {
                p->earnCoins(1);
                logArea->append(QString::fromStdString(p->getName()) + " earned 1 bonus coin (Merchant).");
            }

            // 🧪 Add this debug log:
            logArea->append(QString::fromStdString("🧪 " + p->getName() + " isUnderSanction: " + (p->isUnderSanction() ? "true" : "false")));

            p->setArrestDisabled(false);
            // DO NOT resetUnderSanction here — let gather/tax clear it after blocking
        }

        for (auto* p : players) {
            if (p->getName() == game.turn()) {
                if (p->hasUsedBribe()) {
                    p->resetBribe();
                    logArea->append(QString::fromStdString(p->getName()) + " used bribe and already played one action. Now playing their second.");
                    updatePlayerList();
                    return;
                }
            }
        }

        game.nextTurn();
        updatePlayerList();

    } catch (const std::exception& e) {
        showError(e.what());
    }
}

void CoupGUI::updatePlayerList() {
    logArea->append("🟢 It's " + QString::fromStdString(game.turn()) + "'s turn.");
    playerList->clear();
    for (QWidget* w : playerCards) delete w;
    playerCards.clear();

    for (auto* p : players) {
        QString display = QString::fromStdString(p->getName());
        if (!p->isAlive()) {
            display = "❌ " + display;  // Add ❌ if eliminated
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

QWidget* CoupGUI::createPlayerCard(coup::Player* p) {
    QString role = getRoleName(p);
    QWidget* card = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(card);

    QLabel* emoji = new QLabel(getRoleEmoji(role));
    emoji->setAlignment(Qt::AlignCenter);
    emoji->setStyleSheet("font-size: 28px;");
    emoji->setFont(QFont("Segoe UI Emoji"));  // ✅ Ensure emoji displays

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

coup::Player* CoupGUI::selectTarget(const std::string& currentName, const QString& actionTitle) {
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
    logArea->append("❌ " + QString::fromStdString(msg));  // 👈 Also show in log
    QMessageBox::warning(this, "Error", QString::fromStdString(msg));
}


CoupGUI::~CoupGUI() {
    for (auto* p : players) delete p;
}
