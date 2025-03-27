#include "game.h"
#include <cstdlib> // For srand and rand
#include <ctime>   // For time
int main(int argc, char* argv[]) {
    // Initialize SDL
    if (!initSDL()) {
        return 1;
    }
    srand(static_cast<uint8_t>(time(0)));
    // Define the maze layout using a 2D vector
    /*  0 = WALL
        1 = PATH
        2 = ITEM
        3 = PLAYER_INT
        4 = GOAL_INT
     */
    std::vector<std::vector<int>> mazeLayout = {
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
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0},
    {0,1,1,1,1,1,0,1,1,1,1,2,1,1,1,1,1,1,4,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

    // Initialize game state using the layout
    initializeMaze(mazeLayout);

    // Game loop
    SDL_Event event;
    bool quit = false;
    Uint64 gameStartTime = SDL_GetTicks(); // Track when the game started

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            handleInput(event);
        }

        updateGameState();
        renderMaze();

        // Check win/lose AFTER rendering
        if (gameOver) {
            Uint64 gameEndTime = SDL_GetTicks();
            Uint64 durationMs = gameEndTime - gameStartTime;
            float seconds = durationMs / 1000.0f;

            // Build message
            std::string message;
            std::string title;

            if (gameWon) {
                title = "You Win!";
                char timeBuffer[32];
                snprintf(timeBuffer, sizeof(timeBuffer), "%.2f", seconds);
                message = "Final Score: " + std::to_string(score) +
                    "\nTime: " + std::string(timeBuffer) + "s";
            }
            else {
                title = "You Lose!";
                message = "Final Score: " + std::to_string(score) +
                    "\nTime Remaining: " + std::to_string(health) + "s";
            }

            // Show popup with buttons
            const SDL_MessageBoxButtonData buttons[] = {
                { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Restart" },
                { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Exit" }
            };

            const SDL_MessageBoxData msgboxData = {
                SDL_MESSAGEBOX_INFORMATION,
                window,
                title.c_str(),
                message.c_str(),
                SDL_arraysize(buttons),
                buttons,
                nullptr
            };

            int buttonId = 0;
            SDL_ShowMessageBox(&msgboxData, &buttonId);

            if (buttonId == 1) {
                // Restart logic
                initializeMaze(mazeLayout); // Reinitialize with the same layout
                gameStartTime = SDL_GetTicks();
                gameOver = false;
                gameWon = false;
            }
            else {
                quit = true;
            }
        }
    }
    // Clean up
    closeSDL();
    return 0;
}