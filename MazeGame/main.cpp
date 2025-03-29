// main.cpp
#include "Game.h"
#include "GameConfig.h"
#include "Maze.h"
#include "UIManager.h"

#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[]) {
    // Initialize SDL and game systems
    if (!Game::init()) {
        return 1;   // Exit if initialization fails
    }

    // Seed the random number generator (used for item/enemy placement)
    srand(static_cast<unsigned int>(time(nullptr)));

    // Define the maze layout
    // 0 = Wall, 1 = Path, 2 = Item, 3 = Player, 4 = Goal, 5 = Enemy
    // Make sure the number of columns and rows correspond to MAZE_WIDTH and MAZE_HEIGHT respectively
    std::vector<std::vector<int>> layout = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,3,1,1,1,0,1,1,1,1,1,0,1,1,1,2,0,1,1,0},
        {0,1,0,0,1,0,1,0,0,0,1,0,1,0,0,1,0,0,1,0},
        {0,1,0,2,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,0},
        {0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,1,0},
        {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,0,1,0},
        {0,1,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0},
        {0,1,0,2,1,1,1,2,0,1,1,1,1,2,1,0,1,1,1,0},
        {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0},
        {0,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0},
        {0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0},
        {0,2,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,0},
        {0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0},
        {0,1,1,1,1,1,0,1,1,1,1,2,1,1,1,1,1,1,4,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

    // Load the maze and place items, enemies, player, etc.
    Maze::loadLayout(layout);

    // Setup UI labels like Score, Time, Lives
    UIManager::setupDefaultLabels();

    // Main game loop
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        // Process all SDL events (keyboard, quit, etc.)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;    // User closed window
            }

            // Handle keyboard input (arrow keys)
            Game::handleInput(event);
        }

        // Update game logic (player, enemies, collisions, etc.)
        Game::update();

        // Draw everything on the screen
        Game::render();

        // If game over, show win/lose popup
        if (Game::isOver()) {
            Game::showEndScreen();

            if (Game::shouldQuit()) {
                quit = true;
            }
        }
    }

    // Clean up all SDL resources
    Game::shutdown();
    return 0;
}
