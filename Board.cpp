#include "Board.h"
#include <vector>
#include <cstdint>

// Pre-calculated track path starting from Green's start position (6, 1)
const std::array<Ludo::Coord, Ludo::TRACK_SIZE> Board::TRACK_PATH = {{
    {6, 1}, {6, 2}, {6, 3}, {6, 4}, {6, 5},
    {5, 6}, {4, 6}, {3, 6}, {2, 6}, {1, 6}, {0, 6},
    {0, 7}, {0, 8},
    {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8},
    {6, 9}, {6, 10}, {6, 11}, {6, 12}, {6, 13}, {6, 14},
    {7, 14}, {8, 14},
    {8, 13}, {8, 12}, {8, 11}, {8, 10}, {8, 9},
    {9, 8}, {10, 8}, {11, 8}, {12, 8}, {13, 8}, {14, 8},
    {14, 7}, {14, 6},
    {13, 6}, {12, 6}, {11, 6}, {10, 6}, {9, 6},
    {8, 5}, {8, 4}, {8, 3}, {8, 2}, {8, 1}, {8, 0},
    {7, 0}, {6, 0}
}};

const std::array<std::array<Ludo::Coord, Ludo::HOME_STRETCH_SIZE + 1>, Ludo::MAX_PLAYERS> Board::HOME_STRETCHES = {{
    // P0: Green
    {{{7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}, {7, 6}}},
    // P1: Red
    {{{1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}}},
    // P2: Blue
    {{{7, 13}, {7, 12}, {7, 11}, {7, 10}, {7, 9}, {7, 8}}},
    // P3: Yellow
    {{{13, 7}, {12, 7}, {11, 7}, {10, 7}, {9, 7}, {8, 7}}}
}};

Board::Board() {}

Ludo::Coord Board::getCoord(int8_t playerId, int8_t progress) {
    if (progress < 0) {
        // In base: return some sentinel or specific base coord
        // For simplicity, we can use the start spot but indicate it's in base
        return TRACK_PATH[(playerId * 13) % Ludo::TRACK_SIZE]; 
    }
    if (progress < Ludo::TRACK_SIZE) {
        int idx = (progress + (playerId * 13)) % Ludo::TRACK_SIZE;
        return TRACK_PATH[idx];
    }
    int stretchIdx = progress - Ludo::TRACK_SIZE;
    if (stretchIdx < Ludo::HOME_STRETCH_SIZE + 1) {
        return HOME_STRETCHES[playerId][stretchIdx];
    }
    // Finished: stay at home
    return HOME_STRETCHES[playerId][Ludo::HOME_STRETCH_SIZE];
}

bool Board::isSafeSpot(int8_t r, int8_t c) {
    // Standard safe spots in Ludo
    static const std::vector<std::pair<int8_t, int8_t>> safeSpots = {
        {6, 1}, {8, 3}, {2, 6}, {1, 8}, {6, 12}, {8, 13}, {12, 8}, {13, 6}
    };
    for (const auto& s : safeSpots) {
        if (s.first == r && s.second == c) return true;
    }
    return false;
}