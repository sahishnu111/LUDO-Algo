#include "GameManager.h"
#include <random>
#include <sstream>

GameManager::GameManager() {}

std::string GameManager::generateGameId() {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

    std::string id;
    for (int i = 0; i < 6; ++i) id += alphanum[dis(gen)];
    return id;
}

std::string GameManager::createGame() {
    std::lock_guard<std::mutex> lock(managerMutex);
    std::string id = generateGameId();
    while (games.find(id) != games.end()) {
        id = generateGameId();
    }
    
    auto newGame = std::make_shared<Game>();
    // Pre-populate with 4 players
    newGame->addPlayer(Player(0, "Green", "#2ecc71", false));
    newGame->addPlayer(Player(1, "Red", "#e74c3c", false));
    newGame->addPlayer(Player(2, "Blue", "#3498db", false));
    newGame->addPlayer(Player(3, "Yellow", "#f1c40f", false));
    
    games[id] = newGame;
    return id;
}

std::shared_ptr<Game> GameManager::getGame(const std::string& gameId) {
    std::lock_guard<std::mutex> lock(managerMutex);
    auto it = games.find(gameId);
    return (it != games.end()) ? it->second : nullptr;
}

bool GameManager::removeGame(const std::string& gameId) {
    std::lock_guard<std::mutex> lock(managerMutex);
    return games.erase(gameId) > 0;
}
