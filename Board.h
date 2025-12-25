#ifndef LUDO_GAME_BOARD_H
#define LUDO_GAME_BOARD_H

#include <array>
#include <cstdint>
#include "Constants.h"

class Board {
public:
    Board();
    
    // Get coordinate for a player's piece based on their progress
    static Ludo::Coord getCoord(int8_t playerId, int8_t progress);
    
    // Check if a spot is safe
    static bool isSafeSpot(int8_t r, int8_t c);

private:
    static const std::array<Ludo::Coord, Ludo::TRACK_SIZE> TRACK_PATH;
    static const std::array<std::array<Ludo::Coord, Ludo::HOME_STRETCH_SIZE + 1>, Ludo::MAX_PLAYERS> HOME_STRETCHES;
};

#endif //LUDO_GAME_BOARD_H
