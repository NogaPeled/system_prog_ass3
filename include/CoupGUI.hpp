#ifndef COUP_GUI_HPP
#define COUP_GUI_HPP

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

class CoupGUI : public QWidget {
    Q_OBJECT

public:
    explicit CoupGUI(QWidget* parent = nullptr);
    ~CoupGUI();

private slots:
    void onGather();
    void onTax();
    void onCoup();
    void onSanction();
    void onArrest();
    void onBribe();
    void onSpy();
    void onInvest();
    void onNextTurn();

private:
    coup::Game game;
    std::vector<coup::Player*> players;

    QListWidget* playerList;
    QTextEdit* logArea;
    QGridLayout* cardLayout;
    std::vector<QWidget*> playerCards;

    void updatePlayerList();
    QWidget* createPlayerCard(coup::Player* p);
    coup::Player* selectTarget(const std::string& currentName, const QString& actionTitle);
    QString getRoleName(coup::Player* p);
    QString getRoleEmoji(const QString& role);
    QString getRoleAbility(const QString& role);
    void showError(const std::string& msg);
    void checkForWinner();
    void disableAllButtons();
};

#endif // COUP_GUI_HPP
