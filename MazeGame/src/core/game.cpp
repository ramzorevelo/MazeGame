// game.cpp
#include "Game.h"
#include "GameConfig.h"
#include "Maze.h"
#include "UIManager.h"
#include "VisualEffect.h"
#include "EntityRegistry.h"
#include "EventBus.h"
#include "Events.h"
#include "InputSystem.h"
#include "Components.h"
#include "EntityFactory.h"
#include "AISystem.h"
#include "CollisionSystem.h"
#include "RenderSystem.h"

#include <sstream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// ---------------------
// INTERNAL STATE
// ---------------------
namespace {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    EntityRegistry  registry;
    EventBus        eventBus;
    EntityID        playerEntity{ NULL_ENTITY };

    InputSystem     inputSystem;
    AISystem        aiSystem;
    CollisionSystem collisionSystem;
    RenderSystem    renderSystem;

    bool   gameOver{ false };
    bool   gameWon{ false };
    bool   exitRequested{ false };
    Uint64 gameStartTime{ 0 };

    // Wire all game-level responses to events.
    // Called ONCE in init(). Lambdas read globals from this namespace directly
    // (no capture needed) so they remain correct after a restart.
    void setupEventHandlers() {
        // Move the player if the target tile is walkable
        eventBus.subscribe<MoveActionEvent>([](const MoveActionEvent& e) {
            if (!registry.isAlive(playerEntity)) return;
            auto& pos{ registry.getComponent<PositionComponent>(playerEntity) };
            const int nx{ pos.x + e.dx };
            const int ny{ pos.y + e.dy };
            if (Maze::isWalkable(nx, ny)) {
                pos.x = nx;
                pos.y = ny;
            }
            });

        // Show floating damage text (lives already decremented by CollisionSystem)
        eventBus.subscribe<EnemyHitEvent>([](const EnemyHitEvent&) {
            if (!registry.isAlive(playerEntity)) return;
            const auto& pos{ registry.getComponent<PositionComponent>(playerEntity) };
            VisualEffect::EffectConfig cfg;
            cfg.color = { 255, 0, 0, 255 };
            cfg.fontSize = 28;
            cfg.riseSpeed = 0.08f;
            cfg.duration = 1200;
            VisualEffect::add(
                "-" + std::to_string(GameConfig::LIFE_LOSS_ON_HIT) + " Life",
                pos.x, pos.y, cfg);
            });

        // Add score and show floating points text
        eventBus.subscribe<ItemCollectedEvent>([](const ItemCollectedEvent& e) {
            if (!registry.isAlive(playerEntity)) return;
            auto& sc{ registry.getComponent<ScoreComponent>(playerEntity) };
            sc.score += e.points;
            VisualEffect::EffectConfig cfg;
            cfg.color = { 0, 255, 0, 255 };
            cfg.fontSize = 28;
            cfg.riseSpeed = 0.1f;
            cfg.duration = 1500;
            VisualEffect::add("+" + std::to_string(e.points), e.tileX, e.tileY, cfg);
            });

        // End the level
        eventBus.subscribe<GoalReachedEvent>([](const GoalReachedEvent&) {
            gameWon = true;
            gameOver = true;
            });
    }
} // namespace

// ---------------------
// SDL ACCESS
// ---------------------
SDL_Renderer* Game::getRenderer() { return renderer; }
SDL_Window* Game::getWindow() { return window; }
TTF_Font* Game::getFont() { return font; }

// ---------------------
// INIT
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
    window = SDL_CreateWindow("Maze Game",
        GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT, 0);
    if (!window) { SDL_Log("Window failed: %s", SDL_GetError()); return false; }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) { SDL_Log("Renderer failed: %s", SDL_GetError()); return false; }

    font = TTF_OpenFont(GameConfig::FONT_PATH, GameConfig::FONT_SIZE);
    if (!font) { SDL_Log("Font failed: %s", SDL_GetError()); return false; }

    inputSystem.loadFromJSON("assets/config/input.json");
    setupEventHandlers(); // subscriptions are permanent; call once only

    gameStartTime = SDL_GetTicks();
    return true;
}

// ---------------------
// LOAD LEVEL
// ---------------------
void Game::loadLevel(const std::vector<std::vector<int>>& layout) {
    registry = EntityRegistry{}; // wipe all entities
    playerEntity = NULL_ENTITY;

    Maze::initFromLayout(layout);
    EntityFactory::createAll(registry, layout, playerEntity);
}

// ---------------------
// SHUTDOWN
// ---------------------
void Game::shutdown() {
    TTF_CloseFont(font); font = nullptr;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// ---------------------
// INPUT
// ---------------------
void Game::handleInput(SDL_Event& event) {
    inputSystem.processEvent(event, eventBus);
}

bool Game::shouldQuit() { return exitRequested; }

// ---------------------
// UPDATE
// ---------------------
void Game::update() {
    if (gameOver || !registry.isAlive(playerEntity)) return;

    // Expire invulnerability timers
    collisionSystem.updateInvulnerability(registry);

    // Move enemies and items
    aiSystem.update(registry);

    // Detect all collisions and publish events (events fire synchronously)
    collisionSystem.update(registry, eventBus);

    // Countdown timer
    auto& tm{ registry.getComponent<TimerComponent>(playerEntity) };
    Uint64 now{ SDL_GetTicks() };
    if (now - tm.lastDecrease >= GameConfig::TIME_DECREASE_INTERVAL) {
        tm.remaining--;
        tm.lastDecrease = now;
    }

    // Floating text animations
    VisualEffect::updateAll();

    // Lose conditions
    const auto& hp{ registry.getComponent<HealthComponent>(playerEntity) };
    if (hp.lives <= 0 || tm.remaining <= 0) {
        gameWon = false;
        gameOver = true;
    }
}

// ---------------------
// RENDER
// ---------------------
void Game::render() {
    SDL_SetRenderDrawColor(renderer,
        GameConfig::COLOR_BG.r, GameConfig::COLOR_BG.g,
        GameConfig::COLOR_BG.b, 255);
    SDL_RenderClear(renderer);

    Maze::render();
    renderSystem.render(registry, renderer);
    VisualEffect::renderAll();

    const auto& sc{ registry.getComponent<ScoreComponent>(playerEntity) };
    const auto& tm{ registry.getComponent<TimerComponent>(playerEntity) };
    const auto& hp{registry.getComponent<HealthComponent>(playerEntity)};
    UIManager::renderAll(sc.score, tm.remaining, hp.lives);

    SDL_RenderPresent(renderer);
}

// ---------------------
// GAME STATE
// ---------------------
bool Game::isOver() { return gameOver; }

void Game::showEndScreen() {
    Uint64 endTime{ SDL_GetTicks() };
    float  seconds{ (endTime - gameStartTime) / 1000.0f };

    std::ostringstream ts;
    ts.precision(GameConfig::TIME_PLAYED_DECIMALS);
    ts << std::fixed << seconds;

    const auto& sc{ registry.getComponent<ScoreComponent>(playerEntity) };
    const auto& hp{ registry.getComponent<HealthComponent>(playerEntity)};

    std::string title{ gameWon ? "You Win!" : "You Lose!" };
    std::string message{ "Final Score: " + std::to_string(sc.score) +
        "\nLives Remaining: " + std::to_string(hp.lives) +
        "\nTime Played: " + ts.str() + "s" };

    const SDL_MessageBoxButtonData buttons[]{
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Restart" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Exit"    }
    };
    const SDL_MessageBoxData msgData{
        SDL_MESSAGEBOX_INFORMATION, window,
        title.c_str(), message.c_str(),
        SDL_arraysize(buttons), buttons, nullptr
    };

    int buttonId{ 0 };
    SDL_ShowMessageBox(&msgData, &buttonId);

    if (buttonId == 1) {
        loadLevel(Maze::getOriginalLayout());
        gameOver = false;
        gameWon = false;
        gameStartTime = SDL_GetTicks();
        UIManager::setupDefaultLabels();
    }
    else {
        exitRequested = true;
    }
}