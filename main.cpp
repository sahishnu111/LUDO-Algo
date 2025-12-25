#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "Game.h"
#include "Player.h"
#include "Computer.h"

int main() {
    std::cout << "\tLUDO GAME\t" << std::endl;
    std::cout << "1. Play against Computer (2 Players)" << std::endl;
    std::cout << "2. Multiplayer (2-4 Players)" << std::endl;
    std::cout << "Enter choice (1 or 2): ";
    
    int choice;
    if (!(std::cin >> choice)) {
        std::cout << "Invalid input. Exiting." << std::endl;
        return 0;
    }

    Game game;
    
    if (choice == 1) {
        // Human vs Computer
        // Player 0: Computer (Red)
        // Player 1: Human (Green)
        std::cout << "Starting Game: Computer (Red) vs You (Green)" << std::endl;
        Computer comp(0, "Computer", "Red");
        Player human(1, "Player2", "Green", false);
        
        game.addPlayer(comp);
        game.addPlayer(human);
        
    } else {
        int numPlayers;
        std::cout << "Enter number of players (2-4): ";
        std::cin >> numPlayers;
        if (numPlayers < 2 || numPlayers > 4) {
            std::cout << "Invalid number of players." << std::endl;
            return 0;
        }
        
        std::vector<std::string> colors = {"Red", "Green", "Blue", "Yellow"};
        for (int i = 0; i < numPlayers; i++) {
            std::string name;
            std::cout << "Enter name for Player " << (i + 1) << " (" << colors[i] << "): ";
            std::cin >> name;
            Player p(i, name, colors[i], false);
            game.addPlayer(p);
        }
    }

    if (game.isPlayable()) {
        std::cout << "Game Started!" << std::endl;
        game.startGame();
    } else {
        std::cout << "Game could not start." << std::endl;
    }

    return 0;
}