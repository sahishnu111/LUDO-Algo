#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <map>
#include <string>
#include <memory>
#include <mutex>
#include "Game.h"

class GameManager {
private:
    std::map<std::string, std::shared_ptr<Game>> games;
    std::mutex managerMutex; // Protect access to the map
    
    // Helper to generate IDs
    std::string generateGameId();

public:
    GameManager();
    
    // Create a new game and return its ID
    std::string createGame();
    
    // Get a game instance by ID (thread-safe retrieval)
    std::shared_ptr<Game> getGame(const std::string& gameId);
    
    // Remove a game (e.g., when finished)
    bool removeGame(const std::string& gameId);
};

#endif // GAMEMANAGER_H
