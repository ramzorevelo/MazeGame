// EntityFactory.cpp
#include "EntityFactory.h"
#include "EntityRegistry.h"
#include "Components.h"
#include "GameConfig.h"
#include "Maze.h"
#include <cstdlib>

// Layout tile values (matches main.cpp convention)
static constexpr int TILE_WALL{ 0 };
static constexpr int TILE_PATH{ 1 };
static constexpr int TILE_ITEM{ 2 };
static constexpr int TILE_PLAYER{ 3 };
static constexpr int TILE_GOAL{ 4 };
static constexpr int TILE_ENEMY{ 5 };

EntityID EntityFactory::createPlayer(EntityRegistry& reg, int x, int y) {
    EntityID id{ reg.createEntity() };
    reg.addComponent<PositionComponent>(id, { x, y });
    reg.addComponent<RenderComponent>(id, {
        ShapeType::CIRCLE,
        GameConfig::COLOR_PLAYER_FILL,
        GameConfig::COLOR_PLAYER_OUTLINE,
        0.0f, false
        });
    reg.addComponent<HealthComponent>(id, {
        GameConfig::PLAYER_LIVES, false, 0,
        static_cast<Uint64>(GameConfig::INVULNERABLE_DURATION)
        });
    reg.addComponent<ScoreComponent>(id, { 0 });
    reg.addComponent<TimerComponent>(id, { 100, SDL_GetTicks() });
    reg.addComponent<PlayerTag>(id);
    return id;
}

EntityID EntityFactory::createEnemy(EntityRegistry& reg, int x, int y) {
    EntityID id{ reg.createEntity() };
    reg.addComponent<PositionComponent>(id, { x, y });
    reg.addComponent<RenderComponent>(id, {
        ShapeType::TRIANGLE,
        GameConfig::COLOR_ENEMY_FILL,
        { 0, 0, 0, 255 },
        0.0f, true
        });
    reg.addComponent<AIComponent>(id, {
        static_cast<Direction>(rand() % 4)
        });
    reg.addComponent<MovementComponent>(id, {
        0, static_cast<Uint64>(GameConfig::ENEMY_MOVE_INTERVAL)
        });
    reg.addComponent<EnemyTag>(id);
    return id;
}

EntityID EntityFactory::createItem(EntityRegistry& reg, int x, int y) {
    EntityID id{ reg.createEntity() };
    reg.addComponent<PositionComponent>(id, { x, y });
    reg.addComponent<RenderComponent>(id, {
        ShapeType::CIRCLE,
        GameConfig::COLOR_ITEM_FILL,
        { 0, 0, 0, 255 },
        0.0f, false
        });
    reg.addComponent<AIComponent>(id, {
        static_cast<Direction>(rand() % 4)
        });
    reg.addComponent<MovementComponent>(id, {
        0, static_cast<Uint64>(GameConfig::ITEM_MOVE_INTERVAL)
        });
    reg.addComponent<ItemTag>(id);
    return id;
}

EntityID EntityFactory::createGoal(EntityRegistry& reg, int x, int y) {
    EntityID id{ reg.createEntity() };
    reg.addComponent<PositionComponent>(id, { x, y });
    reg.addComponent<RenderComponent>(id, {
        ShapeType::DIAMOND,
        GameConfig::COLOR_GOAL_FILL,
        { 0, 0, 0, 255 },
        0.0f, false
        });
    reg.addComponent<GoalTag>(id);
    return id;
}

void EntityFactory::createAll(EntityRegistry& reg,
    const std::vector<std::vector<int>>& layout,
    EntityID& outPlayerID) {
    outPlayerID = NULL_ENTITY;
    int enemyCount{ 0 };
    int itemCount{ 0 };

    // First pass: create entities defined in the layout
    for (int y{ 0 }; y < GameConfig::MAZE_HEIGHT; ++y) {
        for (int x{ 0 }; x < GameConfig::MAZE_WIDTH; ++x) {
            switch (layout[y][x]) {
            case TILE_PLAYER:
                outPlayerID = createPlayer(reg, x, y);
                break;
            case TILE_GOAL:
                createGoal(reg, x, y);
                break;
            case TILE_ITEM:
                if (itemCount < GameConfig::MAX_ITEMS) {
                    createItem(reg, x, y);
                    ++itemCount;
                }
                break;
            case TILE_ENEMY:
                if (enemyCount < GameConfig::MAX_ENEMIES) {
                    createEnemy(reg, x, y);
                    ++enemyCount;
                }
                break;
            default:
                break;
            }
        }
    }

    // Second pass: random-fill remaining enemies
    while (enemyCount < GameConfig::MAX_ENEMIES) {
        int x{ rand() % GameConfig::MAZE_WIDTH };
        int y{ rand() % GameConfig::MAZE_HEIGHT };
        if (Maze::isWalkable(x, y)) {
            createEnemy(reg, x, y);
            ++enemyCount;
        }
    }

    // Third pass: random-fill remaining items
    while (itemCount < GameConfig::MAX_ITEMS) {
        int x{ rand() % GameConfig::MAZE_WIDTH };
        int y{ rand() % GameConfig::MAZE_HEIGHT };
        if (Maze::isWalkable(x, y)) {
            createItem(reg, x, y);
            ++itemCount;
        }
    }
}