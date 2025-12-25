#include <iostream>
#include <chrono>
#include "Game.h"
#include "Player.h"

void runBenchmark() {
    Game game;
    game.addPlayer(Player(0, "P0", "Green", false));
    game.addPlayer(Player(1, "P1", "Red", false));
    
    const int iterations = 1000000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        // Simulate a simple move cycle
        game.rollDiceForPlayer(0);
        game.makeMoveForPlayer(0, 0);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    
    std::cout << "Engine Performance Benchmark:" << std::endl;
    std::cout << "Total time for " << iterations << " move cycles: " << diff.count() << " seconds" << std::endl;
    std::cout << "Average time per move cycle: " << (diff.count() / iterations) * 1e6 << " microseconds" << std::endl;
}

int main() {
    runBenchmark();
    return 0;
}
