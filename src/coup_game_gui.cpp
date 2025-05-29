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

    QStringList roles = {"Governor", "Spy", "Baron", "General", "Judge", "Merchant"};
    std::shuffle(roles.begin(), roles.end(), std::mt19937{std::random_device{}()});

    for (int i = 0; i < numPlayers; ++i) {
        QString name = QInputDialog::getText(this, "Player Name", QString("Enter name for player %1:").arg(i + 1), QLineEdit::Normal, "", &ok);
        if (!ok || name.isEmpty()) QApplication::quit();
        QString role = roles[i % roles.size()];
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
                updatePlayerList();
            } catch (const std::exception& e) {
                showError(e.what());
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
                p->tax();
                updatePlayerList();
            } catch (const std::exception& e) {
                showError(e.what());
            }
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

    coup::Player* target = selectTarget(current, "Coup");
    if (!target) return;

    if (getRoleName(target) == "General" && target->getCoins() >= 5) {
        int res = QMessageBox::question(this, "General Coup Block", QString::fromStdString(target->getName()) + ", block the coup for 5 coins?");
        if (res == QMessageBox::Yes) {
            try {
                actor->loseCoins(7);  // Attacker pays cost of coup
                target->loseCoins(5); // General pays to block
                logArea->append(QString::fromStdString(target->getName()) + " blocked the coup!");
                updatePlayerList();
                return;
            } catch (const std::exception& e) {
                showError(e.what());
                return;
            }
        }
    }

    try {
        actor->coup(*target);
        logArea->append(QString::fromStdString(current + " performed a coup on " + target->getName()));
        updatePlayerList();
    } catch (const std::exception& e) {
        showError(e.what());
    }
}


void CoupGUI::onSanction() {
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current) {
            auto* target = selectTarget(current, "Sanction");
            if (!target) return;
            try {
                p->sanction(*target);
                logArea->append(QString::fromStdString(current + " sanctioned " + target->getName()));
                updatePlayerList();
            } catch (const std::exception& e) {
                showError(e.what());
            }
            return;
        }
    }
}

void CoupGUI::onArrest() {
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current) {
            auto* target = selectTarget(current, "Arrest");
            if (!target) return;
            try {
                p->arrest(*target);
                logArea->append(QString::fromStdString(current + " arrested " + target->getName()));
                updatePlayerList();
            } catch (const std::exception& e) {
                showError(e.what());
            }
            return;
        }
    }
}

void CoupGUI::onBribe() {
    auto current = game.turn();
    for (auto* p : players) {
        if (p->getName() == current) {
            try {
                p->bribe();
                logArea->append(QString::fromStdString(current + " used a bribe!"));
                updatePlayerList();
            } catch (const std::exception& e) {
                showError(e.what());
            }
            return;
        }
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

            target->setBlockedFromArrest(true);  // ✅ This blocks target's arrest next turn
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
            p->setBlockedFromArrest(false);
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
        if (p->getName() == game.turn()) {
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
    QMessageBox::warning(this, "Error", QString::fromStdString(msg));
}

CoupGUI::~CoupGUI() {
    for (auto* p : players) delete p;
}
