// Game.h
#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace Game {
    // Initialize SDL, window, renderer, font, etc.
    bool init();

    bool shouldQuit();

    // Shutdown and clean up resources
    void shutdown();

    // Handle keyboard input
    void handleInput(SDL_Event& event);

    // Update game state (movement, check win/lose, etc.)
    void update();

    // Render everything on screen
    void render();

    // Game over state
    bool isOver();

    // Show win/lose popup and handle restart/exit
    void showEndScreen();

    // Access to SDL objects (used in other modules)
    SDL_Renderer* getRenderer();
    SDL_Window* getWindow();
    TTF_Font* getFont();
}