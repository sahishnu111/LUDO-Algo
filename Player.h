#ifndef LUDO_GAME_PLAYER_H
#define LUDO_GAME_PLAYER_H

#include <string>
#include <array>
#include <cstdint>
#include "Constants.h"

class Player {
public:
    int8_t playerId;
    std::string playerName;
    std::string playerColor;
    bool isBot;

    // HFT optimization: use fixed size arrays and small types
    // progress: -1 (base), 0-51 (track), 52-56 (home stretch), 57 (home)
    std::array<int8_t, Ludo::MAX_PIECES> pieceProgress;
    std::array<bool, Ludo::MAX_PIECES> isAtHome; // Reached final 57

    Player(int8_t id, std::string name, std::string color, bool isBotPlayer)
            : playerId(id), playerName(name), playerColor(color), isBot(isBotPlayer) {
        pieceProgress.fill(-1); // -1 means in base
        isAtHome.fill(false);
    }

    int8_t getId() const { return playerId; }
    const std::string& getName() const { return playerName; }
    const std::string& getColor() const { return playerColor; }
    
    void resetPiece(int8_t index) {
        if (index >= 0 && index < Ludo::MAX_PIECES) {
            pieceProgress[index] = -1;
            isAtHome[index] = false;
        }
    }
};

#endif //LUDO_GAME_PLAYER_H
