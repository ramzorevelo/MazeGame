// game.h — full replacement
#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>

namespace Game {
    bool init();
    bool shouldQuit();
    void shutdown();

    void handleInput(SDL_Event& event);
    void update();
    void render();

    bool isOver();
    void showEndScreen();

    // NEW: Load a level from a tile layout. Call once after init(), then again on restart.
    void loadLevel(const std::vector<std::vector<int>>& layout);

    SDL_Renderer* getRenderer();
    SDL_Window* getWindow();
    TTF_Font* getFont();
}