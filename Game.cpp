#include "Game.h"
#include <algorithm>
#include <iostream>

Game::Game() : state(State::WAITING_FOR_PLAYERS) {
    std::random_device rd;
    rng.seed(rd());
    trackOccupancy.fill({-1, 0});
}

bool Game::addPlayer(const Player &player) {
    std::lock_guard<std::recursive_mutex> lock(gameMutex);
    if (players.size() >= Ludo::MAX_PLAYERS) return false;
    players.push_back(player);
    if (players.size() >= 2) state = State::WAITING_FOR_ROLL;
    return true;
}

int8_t Game::generateRandomNumber() {
    std::uniform_int_distribution<> distrib(1, 6);
    return static_cast<int8_t>(distrib(rng));
}

void Game::nextTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    state = State::WAITING_FOR_ROLL;
}

void Game::handleCapture(int8_t playerIdx, int8_t progress) {
    if (progress < 0 || progress >= Ludo::TRACK_SIZE) return;

    // Convert local progress to global track index
    int globalIdx = (progress + (playerIdx * 13)) % Ludo::TRACK_SIZE;
    
    // Safety check: is it a safe spot?
    Ludo::Coord coord = Board::getCoord(playerIdx, progress);
    if (Board::isSafeSpot(coord.r, coord.c)) return;

    auto& occupant = trackOccupancy[globalIdx];
    if (occupant.playerIdx != -1 && occupant.playerIdx != playerIdx) {
        // Capture!
        std::cout << "HFT capture at track " << globalIdx << " by P" << (int)playerIdx << std::endl;
        
        // Reset all pieces of the other player on this square
        for (int i = 0; i < Ludo::MAX_PIECES; i++) {
            int otherProgress = players[occupant.playerIdx].pieceProgress[i];
            if (otherProgress >= 0 && otherProgress < Ludo::TRACK_SIZE) {
                int otherGlobal = (otherProgress + (occupant.playerIdx * 13)) % Ludo::TRACK_SIZE;
                if (otherGlobal == globalIdx) {
                    players[occupant.playerIdx].resetPiece(i);
                }
            }
        }
        occupant.playerIdx = playerIdx;
        occupant.piecesCount = 1; // The capturing piece
    }
}

bool Game::hasPossibleMoves(int8_t pIdx, int8_t roll) const {
    const auto& p = players[pIdx];
    for (int i = 0; i < Ludo::MAX_PIECES; i++) {
        if (p.pieceProgress[i] == -1) {
            if (roll == 6) return true;
        } else if (!p.isAtHome[i]) {
            if (p.pieceProgress[i] + roll < Ludo::TOTAL_PROGRESS_STEPS) return true;
        }
    }
    return false;
}

int8_t Game::rollDiceForPlayer(int8_t pIdx) {
    std::lock_guard<std::recursive_mutex> lock(gameMutex);
    if (state != State::WAITING_FOR_ROLL || pIdx != currentPlayerIndex) return -1;

    currentRoll = generateRandomNumber();
    if (hasPossibleMoves(pIdx, currentRoll)) {
        state = State::WAITING_FOR_MOVE;
    } else {
        // Skip turn
        nextTurn();
    }
    return currentRoll;
}

bool Game::makeMoveForPlayer(int8_t pIdx, int8_t pieceIdx) {
    std::lock_guard<std::recursive_mutex> lock(gameMutex);
    if (state != State::WAITING_FOR_MOVE || pIdx != currentPlayerIndex) return false;
    if (pieceIdx < 0 || pieceIdx >= Ludo::MAX_PIECES) return false;

    Player& p = players[pIdx];
    int8_t currentProg = p.pieceProgress[pieceIdx];

    // Case 1: Spawn
    if (currentProg == -1) {
        if (currentRoll != 6) return false;
        p.pieceProgress[pieceIdx] = 0;
    } 
    // Case 2: Move
    else if (!p.isAtHome[pieceIdx]) {
        int8_t nextProg = currentProg + currentRoll;
        if (nextProg >= Ludo::TOTAL_PROGRESS_STEPS) return false;
        
        // Remove from old global occupancy if on track
        if (currentProg < Ludo::TRACK_SIZE) {
            int oldGlobal = (currentProg + (pIdx * 13)) % Ludo::TRACK_SIZE;
            if (--trackOccupancy[oldGlobal].piecesCount <= 0) {
                trackOccupancy[oldGlobal].playerIdx = -1;
            }
        }

        p.pieceProgress[pieceIdx] = nextProg;
        if (nextProg == Ludo::TOTAL_PROGRESS_STEPS - 1) {
            p.isAtHome[pieceIdx] = true;
        }

        // Handle collision/capture if on track
        if (nextProg < Ludo::TRACK_SIZE) {
            handleCapture(pIdx, nextProg);
            int newGlobal = (nextProg + (pIdx * 13)) % Ludo::TRACK_SIZE;
            trackOccupancy[newGlobal].playerIdx = pIdx;
            trackOccupancy[newGlobal].piecesCount++;
        }
    } else {
        return false; // Already home
    }

    checkWinCondition();
    if (state != State::GAME_OVER) {
        if (currentRoll == 6) {
            state = State::WAITING_FOR_ROLL; // Same player rolls again
        } else {
            nextTurn();
        }
    }
    return true;
}

void Game::checkWinCondition() {
    for (const auto& p : players) {
        bool allHome = true;
        for (bool home : p.isAtHome) {
            if (!home) { allHome = false; break; }
        }
        if (allHome) {
            winnerId = p.getId();
            state = State::GAME_OVER;
            return;
        }
    }
}

json Game::getGameState() const {
    std::lock_guard<std::recursive_mutex> lock(gameMutex);
    json j;
    j["current_turn"] = currentPlayerIndex;
    j["last_roll"] = currentRoll;
    j["state"] = (int)state;
    j["winner"] = winnerId;
    j["players"] = json::array();
    
    for (const auto& p : players) {
        json pj;
        pj["id"] = p.getId();
        pj["name"] = p.getName();
        pj["color"] = p.getColor();
        pj["pieces"] = json::array();
        for (int i = 0; i < Ludo::MAX_PIECES; i++) {
            Ludo::Coord c = Board::getCoord(p.getId(), p.pieceProgress[i]);
            pj["pieces"].push_back({
                {"row", c.r},
                {"col", c.c},
                {"progress", p.pieceProgress[i]},
                {"home", p.isAtHome[i]}
            });
        }
        j["players"].push_back(pj);
    }
    return j;
}

void Game::resetGame() {
    std::lock_guard<std::recursive_mutex> lock(gameMutex);
    for (auto& p : players) {
        for (int i = 0; i < Ludo::MAX_PIECES; i++) p.resetPiece(i);
    }
    trackOccupancy.fill({-1, 0});
    currentPlayerIndex = 0;
    currentRoll = 0;
    winnerId = -1;
    state = State::WAITING_FOR_ROLL;
}
