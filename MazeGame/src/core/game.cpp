// Game.cpp
#include "Game.h"
#include "GameConfig.h"
#include "Renderer.h"
#include "Maze.h"
#include "UIManager.h"
#include "VisualEffect.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "Goal.h"

// ---------------------
// INTERNAL GAME STATE
// ---------------------
namespace {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    bool gameOver = false;
    bool gameWon = false;
    bool exitRequested = false;

    int score = 0;
    int timeRemaining = 100;
    int playerLives = GameConfig::PLAYER_LIVES;

    Uint64 gameStartTime = 0;
    Uint64 lastTimeDecrease = 0;

    // For invulnerability after being hit
    bool isInvulnerable = false;
    Uint64 invulnerableStartTime = 0;
}

// ---------------------
// SDL ACCESS FUNCTIONS
// These are used by other modules
// ---------------------
SDL_Renderer* Game::getRenderer() { return renderer; }
SDL_Window* Game::getWindow() { return window; }
TTF_Font* Game::getFont() { return font; }


// ---------------------
// INITIALIZATION
// ---------------------
bool Game::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL Init failed: %s", SDL_GetError());
        return false;
    }

    if (!TTF_Init()) {
        SDL_Log("TTF Init failed: %s", SDL_GetError());
        return false;
    }

    // Create window and renderer
    window = SDL_CreateWindow("Maze Game", GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT, 0);
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return false;
    }

    // Load font
    font = TTF_OpenFont(GameConfig::FONT_PATH, GameConfig::FONT_SIZE);
    if (!font) {
        SDL_Log("Font load failed: %s", SDL_GetError());
        return false;
    }

    // Record start time
    gameStartTime = SDL_GetTicks();
    lastTimeDecrease = gameStartTime;

    return true;
}

// ---------------------
// CLEANUP
// ---------------------
void Game::shutdown() {
    TTF_CloseFont(font);
    font = nullptr;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}


// ---------------------
// INPUT HANDLING
// ---------------------
void Game::handleInput(SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
        case SDLK_UP: Player::move(0, -1); break;
        case SDLK_DOWN: Player::move(0, 1); break;
        case SDLK_LEFT: Player::move(-1, 0); break;
        case SDLK_RIGHT: Player::move(1, 0); break;
        }
    }
}

bool Game::shouldQuit() {
    return exitRequested;
}



// ---------------------
// GAME LOGIC UPDATE
// ---------------------
void Game::update() {
    if (gameOver) return;

    Uint64 now = SDL_GetTicks();

    // Handle invulnerability timer
    if (isInvulnerable && now - invulnerableStartTime >= GameConfig::INVULNERABLE_DURATION) {
        isInvulnerable = false;
    }

    // Decrease timer every second
    if (now - lastTimeDecrease >= GameConfig::TIME_DECREASE_INTERVAL) {
        timeRemaining--;
        lastTimeDecrease = now;
    }

    // Update game objects
    Player::update();
    Enemy::updateAll();
    Item::updateAll();
    VisualEffect::updateAll();

    // Check for item pickup
    Item::checkCollection(score);

    // Check for enemy collision
    if (!isInvulnerable && Enemy::checkCollisionWithPlayer()) {
        playerLives -= GameConfig::LIFE_LOSS_ON_HIT;
        isInvulnerable = true;
        invulnerableStartTime = now;

        VisualEffect::EffectConfig config;
        config.color = { 255, 0, 0, 255 }; // Red for damage
        config.fontSize = 28;
        config.riseSpeed = 0.08f;
        config.duration = 1200;

        VisualEffect::add("-" + std::to_string(GameConfig::LIFE_LOSS_ON_HIT) + " Life",
            Player::getX(), Player::getY(), config);
    }

    // Win condition
    if (Goal::checkReached()) {
        gameWon = true;
        gameOver = true;
    }

    // Lose condition
    if (timeRemaining <= 0 || playerLives <= 0) {
        gameWon = false;
        gameOver = true;
    }
}

// ---------------------
// RENDER EVERYTHING
// ---------------------
void Game::render() {
    SDL_SetRenderDrawColor(renderer, GameConfig::COLOR_BG.r, GameConfig::COLOR_BG.g, GameConfig::COLOR_BG.b, 255);
    SDL_RenderClear(renderer);

    // Draw game components
    Maze::render();
    Item::renderAll();
    Enemy::renderAll();
    Goal::render();
    Player::render(isInvulnerable);
    VisualEffect::renderAll();
    UIManager::renderAll(score, timeRemaining, playerLives);

    SDL_RenderPresent(renderer);
}

// ---------------------
// END SCREEN POPUP
// ---------------------
bool Game::isOver() {
    return gameOver;
}

void Game::showEndScreen() {
    Uint64 endTime = SDL_GetTicks();
    float seconds = (endTime - gameStartTime) / 1000.0f;

    // Format time with configurable decimal places
    std::ostringstream timeStream;
    timeStream.precision(GameConfig::TIME_PLAYED_DECIMALS);
    timeStream << std::fixed << seconds;

    std::string title = gameWon ? "You Win!" : "You Lose!";
    std::string message = "Final Score: " + std::to_string(score) +
        "\nLives Remaining: " + std::to_string(playerLives) +
        "\nTime Played: " + timeStream.str() + "s";

    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Restart" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Exit" }
    };

    const SDL_MessageBoxData msgData = {
        SDL_MESSAGEBOX_INFORMATION,
        window, title.c_str(), message.c_str(),
        SDL_arraysize(buttons), buttons, nullptr
    };

    int buttonId = 0;
    SDL_ShowMessageBox(&msgData, &buttonId);

    if (buttonId == 1) {
        // Restart the game
        Maze::reload();
        gameOver = false;
        gameWon = false;
        timeRemaining = 100;
        playerLives = GameConfig::PLAYER_LIVES;
        score = 0;
        gameStartTime = SDL_GetTicks();
        lastTimeDecrease = gameStartTime;
        UIManager::setupDefaultLabels();
    }
    else {
        // Exit the game
        exitRequested = true;
    }
}