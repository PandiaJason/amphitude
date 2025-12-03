#include "Game.h"
#include <cstdlib>
#include <ctime>

/**
 * @brief Entry point of the application.
 * 
 * Sets up the random seed, creates the Game instance, and starts the loop.
 */
int main(int argc, char* argv[]) {
    // Seed the random number generator with the current time
    // This ensures random events (like power-up spawns) are different each run.
    srand(static_cast<unsigned>(time(0)));

    Game game;
    
    // Initialize the game (SDL, Window, Assets)
    if (game.init()) {
        // Run the main game loop
        game.run();
    }

    return 0;
}
