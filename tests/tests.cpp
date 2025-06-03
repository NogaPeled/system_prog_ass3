//nogapeled19@gmail.com
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../include/Game.hpp"
#include "../include/Governor.hpp"
#include "../include/Spy.hpp"
#include "../include/Baron.hpp"
#include "../include/General.hpp"
#include "../include/Judge.hpp"
#include "../include/Merchant.hpp"

using namespace coup;

TEST_CASE("Basic coin management and gather") {
    Game game;
    Governor g(game, "Alice");
    CHECK(g.getCoins() == 0);
    g.gather();
    CHECK(g.getCoins() == 1);
}

TEST_CASE("Governor: tax") {
    Game game;
    Governor gov1(game, "Alice");
    Governor gov2(game, "Bob");
    Governor gov3(game, "Charlie");

    CHECK(game.turn() == "Alice");
    CHECK_NOTHROW(gov1.tax());
    CHECK(gov1.getCoins() == 3);

    while (game.turn() != "Bob") {
        game.nextTurn();
    }

    CHECK(game.turn() == "Bob");
    CHECK_NOTHROW(gov2.tax());
    CHECK(gov2.getCoins() == 3);
}

TEST_CASE("Spy watch and clear") {
    Game game;
    Spy spy(game, "S");
    Governor target(game, "T");

    spy.watch(target);
    CHECK(spy.getBlockedTarget() == &target);
    spy.clearWatch();
    CHECK(spy.getBlockedTarget() == nullptr);
}

TEST_CASE("Baron invest and sanction gain") {
    Game game;
    Baron b(game, "B");
    b.earnCoins(5);
    b.invest();  // costs 3, adds 6
    CHECK(b.getCoins() == 8);
    b.onSanction();  // +1
    CHECK(b.getCoins() == 9);
}

TEST_CASE("General block coup and arrest reward") {
    Game game;
    General g(game, "G");
    g.earnCoins(5);
    g.blockCoup();  // pays 5
    CHECK(g.getCoins() == 0);
    g.onArrest();  // +1
    CHECK(g.getCoins() == 1);
}

TEST_CASE("Judge on sanction") {
    Game game;
    Judge j(game, "J");
    Governor a(game, "A");
    a.earnCoins(5);
    game.nextTurn();  // turn: A
    CHECK_NOTHROW(a.sanction(j, 4));
    CHECK(a.getCoins() == 0);
    CHECK(j.isAlive());
}

TEST_CASE("Merchant passive income and arrest") {
    Game game;
    Merchant m(game, "M");
    m.earnCoins(3);
    m.onStartTurn();  // +1
    CHECK(m.getCoins() == 4);
    m.onArrest();  // -2
    CHECK(m.getCoins() == 2);
}

TEST_CASE("Bribe and action usage") {
    Game game;
    Governor g(game, "G");
    g.earnCoins(5);
    g.bribe();  // -4, +1 action
    CHECK(g.getCoins() == 1);
    g.useAction();
    g.useAction();
    CHECK(g.getActionsLeft() == 0);
}

TEST_CASE("Spy blocks one arrest only") {
    Game game;
    Spy spy(game, "S");
    Governor a(game, "A");
    spy.watch(a);
    CHECK_THROWS_WITH(spy.preventNextArrest(a), "A's arrest was blocked by Spy!");
}

TEST_CASE("Coup must be done when over limit") {
    Game game;
    Governor a(game, "A");
    Governor b(game, "B");
    a.earnCoins(10);
    CHECK_THROWS_WITH(a.gather(), "A has 10 or more coins and must perform a coup.");
    CHECK_NOTHROW(a.coup(b));
    CHECK_FALSE(b.isAlive());
}

TEST_CASE("Sanctioned player cannot gather or tax") {
    Game game;
    Governor a(game, "A");
    Governor b(game, "B");

    b.earnCoins(3);
    game.nextTurn();  // turn: B
    b.sanction(a, 3);

    game.nextTurn();  // turn: A
    CHECK_THROWS_WITH(a.gather(), "You are under sanction and cannot gather.");
    CHECK_THROWS_WITH(a.tax(), "You are under sanction and cannot tax.");
}

TEST_CASE("Coup skips eliminated players") {
    Game game;
    Governor a(game, "A");
    Governor b(game, "B");
    Governor c(game, "C");

    a.earnCoins(10);
    a.coup(b);
    CHECK_FALSE(b.isAlive());
    game.nextTurn();  // B skipped
    CHECK(game.turn() == "C");
}

TEST_CASE("Game winner check") {
    Game game;
    Governor a(game, "A");
    Governor b(game, "B");
    a.earnCoins(10);
    a.coup(b);
    CHECK(game.winner() == "A");
}

TEST_CASE("Cannot coup or arrest dead player") {
    Game game;
    Governor a(game, "A");
    Governor b(game, "B");
    a.earnCoins(10);
    a.coup(b);

    Governor c(game, "C");
    c.earnCoins(10);
    CHECK_THROWS(c.coup(b));
    CHECK_THROWS(c.arrest(b));
}

TEST_CASE("Edge: invalid coin operations") {
    Game game;
    Governor g(game, "G");
    CHECK_THROWS(g.earnCoins(-1));
    CHECK_THROWS(g.loseCoins(1));  // has 0
    g.earnCoins(3);
    CHECK_THROWS(g.loseCoins(4));
}
