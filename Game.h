#ifndef LUDO_GAME_GAME_H
#define LUDO_GAME_GAME_H

#include "Player.h"
#include "Board.h"
#include "Constants.h"
#include <vector>
#include <array>
#include <random>
#include <mutex>
#include "libs/json.hpp"

using json = nlohmann::json;

class Game {
public:
    enum class State {
        WAITING_FOR_PLAYERS,
        WAITING_FOR_ROLL,
        WAITING_FOR_MOVE,
        GAME_OVER
    };

private:
    std::vector<Player> players;
    int8_t currentPlayerIndex = 0;
    int8_t currentRoll = 0;
    int8_t winnerId = -1;
    State state = State::WAITING_FOR_PLAYERS;

    std::mt19937 rng;
    mutable std::recursive_mutex gameMutex;

    // Fast mapping for collision detection: track_index -> list of piece info
    // However, in Ludo only one piece (or stacked pieces of SAME player) can be on a square.
    // Except for safe spots.
    struct SquareInfo {
        int8_t playerIdx = -1;
        int8_t piecesCount = 0;
    };
    std::array<SquareInfo, Ludo::TRACK_SIZE> trackOccupancy;

    // Helper functions
    int8_t generateRandomNumber();
    void nextTurn();
    void handleCapture(int8_t playerIdx, int8_t progress);
    void checkWinCondition();

public:
    Game();
    
    // Core Game Actions
    bool addPlayer(const Player &player);
    void startGame();
    int8_t rollDiceForPlayer(int8_t pIdx);
    bool makeMoveForPlayer(int8_t pIdx, int8_t pieceIdx);
    void resetGame();

    // API Helpers
    json getGameState() const;
    int8_t getCurrentPlayer() const { return currentPlayerIndex; }
    State getGameStateEnum() const { return state; }

    // Optimization: Pre-check if any moves are possible
    bool hasPossibleMoves(int8_t pIdx, int8_t roll) const;
};

#endif //LUDO_GAME_GAME_H
