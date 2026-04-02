// Components.h
#pragma once
#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>
#include "ShapeRenderer.h"
#include "Direction.h"

// -------------------------------------------------------
// Serialization helpers for SDL/engine types
// -------------------------------------------------------
inline void to_json(nlohmann::json& j, const SDL_Color& c) {
    j = nlohmann::json{ {"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a} };
}
inline void from_json(const nlohmann::json& j, SDL_Color& c) {
    j.at("r").get_to(c.r); j.at("g").get_to(c.g);
    j.at("b").get_to(c.b); j.at("a").get_to(c.a);
}

NLOHMANN_JSON_SERIALIZE_ENUM(ShapeType, {
    { ShapeType::CIRCLE,   "circle"   },
    { ShapeType::TRIANGLE, "triangle" },
    { ShapeType::DIAMOND,  "diamond"  }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(Direction, {
        { Direction::UP,    "up"    },
        { Direction::RIGHT, "right" },
        { Direction::DOWN,  "down"  },
        { Direction::LEFT,  "left"  }
        })

    // -------------------------------------------------------
    // PositionComponent — tile-grid position
    // -------------------------------------------------------
    struct PositionComponent {
    int x{ 0 };
    int y{ 0 };
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PositionComponent, x, y)

// -------------------------------------------------------
// MovementComponent — used by timed movers (enemies, items)
// -------------------------------------------------------
struct MovementComponent {
    Uint64 lastMoveTime{ 0 };
    Uint64 moveInterval{ 500 }; // ms between moves
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MovementComponent, lastMoveTime, moveInterval)

// -------------------------------------------------------
// RenderComponent — visual representation
// -------------------------------------------------------
struct RenderComponent {
    ShapeType shape{ ShapeType::CIRCLE };
    SDL_Color fill{ 255, 255, 255, 255 };
    SDL_Color outline{ 0, 0, 0, 255 };
    float     rotation{ 0.0f };
    bool      rotEnabled{ false };
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RenderComponent, shape, fill, outline, rotation, rotEnabled)

// -------------------------------------------------------
// HealthComponent — lives + invulnerability window
// -------------------------------------------------------
struct HealthComponent {
    int    lives{ 5 };
    bool   invulnerable{ false };
    Uint64 invulnerableStart{ 0 };
    Uint64 invulnerableDuration = 1000; // ms
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HealthComponent, lives, invulnerable,
    invulnerableStart, invulnerableDuration)

    // -------------------------------------------------------
    // AIComponent — directional AI for random-bounce movers
    // -------------------------------------------------------
    struct AIComponent {
    Direction direction = Direction::UP;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AIComponent, direction)

// -------------------------------------------------------
// ScoreComponent — accumulated score (on player entity)
// -------------------------------------------------------
struct ScoreComponent {
    int score = 0;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ScoreComponent, score)

// -------------------------------------------------------
// TimerComponent — level countdown (on player entity)
// -------------------------------------------------------
struct TimerComponent {
    int    remaining{ 100 };
    Uint64 lastDecrease{ 0 };
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TimerComponent, remaining, lastDecrease)

// -------------------------------------------------------
// Tag components — identity markers
// -------------------------------------------------------
struct PlayerTag {};
struct EnemyTag {};
struct ItemTag {};
struct GoalTag {};

inline void to_json(nlohmann::json& j, const PlayerTag&) { j = {}; }
inline void from_json(const nlohmann::json&, PlayerTag&) {}
inline void to_json(nlohmann::json& j, const EnemyTag&) { j = {}; }
inline void from_json(const nlohmann::json&, EnemyTag&) {}
inline void to_json(nlohmann::json& j, const ItemTag&) { j = {}; }
inline void from_json(const nlohmann::json&, ItemTag&) {}
inline void to_json(nlohmann::json& j, const GoalTag&) { j = {}; }
inline void from_json(const nlohmann::json&, GoalTag&) {}