#ifndef LUDO_GAME_COMPUTER_H
#define LUDO_GAME_COMPUTER_H

#include <iostream>
#include "Player.h"
#include "Board.h"


class Computer : public Player {
public:
    Computer(int id, std::string name, std::string color) : Player(id, name, color, true) {
        playerName = "Computer";
        // Set other attributes as needed
    }


};

#endif //LUDO_GAME_COMPUTER_H