// Direction.h
#pragma once

enum class Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// Helper to get angle in degrees
inline float directionToAngle(Direction dir) {
    switch (dir) {
    case Direction::UP: return 0.0f;
    case Direction::RIGHT: return 90.0f;
    case Direction::DOWN: return 180.0f;
    case Direction::LEFT: return 270.0f;
    }
    return 0.0f; // default
}