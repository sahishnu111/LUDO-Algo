#ifndef LUDO_GAME_CONSTANTS_H
#define LUDO_GAME_CONSTANTS_H

#include <array>
#include <utility>

namespace Ludo {
    constexpr int TRACK_SIZE = 52;
    constexpr int HOME_STRETCH_SIZE = 5;
    constexpr int TOTAL_PROGRESS_STEPS = TRACK_SIZE + HOME_STRETCH_SIZE + 1; // 58 steps total
    constexpr int MAX_PIECES = 4;
    constexpr int MAX_PLAYERS = 4;

    // Coordinate mapping for the 52 track squares
    // We'll pre-fill this or use a helper. 
    // For now, let's just define the structure for HFT-level speed.
    struct Coord {
        int8_t r;
        int8_t c;
    };

    // Fast mapping from progress to coordinates
    extern const std::array<std::array<Coord, TOTAL_PROGRESS_STEPS>, MAX_PLAYERS> PROGRESS_TO_COORD;
}

#endif //LUDO_GAME_CONSTANTS_H
