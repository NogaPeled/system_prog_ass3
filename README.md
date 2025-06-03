# Coup Game – System Programming Project (Assignment 3)

## Project Summary

This project is a C++ implementation of the board game Coup, developed for a system programming course. The goal is to simulate a strategic game where 2–6 players take turns using actions based on their roles, trying to eliminate each other. The project uses object-oriented design principles, including inheritance and polymorphism, and includes unit testing and a GUI built with Qt.

## Structure and Components

### Core Classes (in `src/` and `include/` folders)

- `Game` – Manages the current state of the game, turn order, and active players.
- `Player` – Abstract base class representing any player in the game.
- `Governor` – Gains 3 coins when taxing instead of 2; can block tax.
- `Spy` – Can view another player’s coins and block their next arrest.
- `Baron` – Can invest 3 coins to gain 6; gains 1 coin if sanctioned.
- `General` – Can pay 5 coins to block a coup against any player; regains 1 coin if arrested.
- `Judge` – Can block a bribe, causing the player to lose 4 coins; imposes penalty if sanctioned.
- `Merchant` – If starting a turn with 3 or more coins, gains 1 bonus coin; pays 2 coins to the bank if arrested.

## Gameplay Mechanics

- Actions available to all players (regardless of role):
  - `gather` – Take 1 coin from the bank.
  - `tax` – Take 2 coins from the bank (special roles may modify/block).
  - `bribe` – Pay 4 coins to gain one extra action during this turn.
  - `arrest` – Take 1 coin from another player (can’t be repeated consecutively on the same target).
  - `sanction` – Prevent another player from using gather or tax until their next turn. Costs 3 coins.
  - `coup` – Eliminate a player. Costs 7 coins. Can be blocked only by the General.

- A player with 10 or more coins must perform a coup on their turn.
- Players who are eliminated are skipped in the turn rotation.
- The last remaining player is declared the winner.

## GUI (Qt-based)

The project includes a full graphical user interface using Qt5 Widgets, allowing all players to play from a shared screen. The GUI provides:

- A dynamic player list showing each player's name, role, coin count, and whether they are alive or eliminated.
- Turn indication, highlighting whose turn it is to act.
- Action buttons for all standard actions: gather, tax, bribe, arrest, sanction, and coup.
- A scrolling game log that displays all actions taken, including special ability use and status effects.

### Role-specific GUI Interactions and Popups:

- **Spy**: Can choose a player to watch. A popup appears allowing selection, after which the Spy sees the coin count and may block the player's next arrest.
- **General**: When a player attempts a coup, a popup allows the Generals to decide whether to block it by paying 5 coins.
- **Judge**: When a player uses bribe, the Judges can respond during the current player's turn and cause them to lose 4 coins.
- **Baron**: Gets an "Invest" button during their turn if they have enough coins.
- **Governor**: Can block other players’ tax actions.
- **Merchant**: Automatically gains a bonus coin if they start their turn with at least 3 coins.

All popups and GUI effects are fully integrated with the logic layer and game state. After every action, the interface updates in real-time and reflects the current state of the game.

## Unit Testing

All core functionality is tested using the doctest library.

Location: `tests/tests.cpp`

The tests cover:
- Basic coin mechanics
- Role abilities and their edge cases
- Turn progression and skipping eliminated players
- Enforced coup when holding 10+ coins
- Exceptions for illegal actions

To compile and run the tests:
```
make test
./test_runner
```

## Build Instructions

This project includes a Makefile with the following targets:

- `make test` – Compile and run unit tests
- `make Main` – Compile and launch GUI game demo
- `make valgrind` – Run memory checks using Valgrind
- `make clean` – Remove all build artifacts

## Project Files

- `src/` – Implementation files (.cpp) for all game logic and roles
- `include/` – Header files (.hpp) for class declarations
- `tests/` – Unit tests
- `main.cpp` – Demo game execution
- `Makefile` – Build automation
- `README.md` – This documentation

## Notes

- Exception messages are informative and used for testing expected failures.
- All special role interactions are implemented and tested.
- GUI assumes all players interact from one screen (local game).
- Code was verified with Valgrind to ensure no memory leaks.

## Author

Noga Peled  
System Programming 2 Course  
Ariel University
