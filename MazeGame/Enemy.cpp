// Enemy.cpp
#include "Enemy.h"
#include "Maze.h"
#include "Player.h"
#include "GameConfig.h"
#include <cstdlib>
#include "Direction.h"
namespace {
    struct EnemyData : Entity {
        Direction direction;

        EnemyData(int x, int y)
            : Entity(x, y, ShapeType::TRIANGLE, GameConfig::COLOR_ENEMY_FILL, { 0, 0, 0, 255 }) {
            direction = static_cast<Direction>(rand() % 4);
            setDirection(direction);
            enableRotation(true); // Enable rotation
        }
    };

    std::vector<EnemyData> enemies;
    Uint64 lastMoveTime = 0;

    // Convert dx/dy to Direction
    Direction getDirectionFromDelta(int dx, int dy) {
        if (dx == 1) return Direction::RIGHT;
        if (dx == -1) return Direction::LEFT;
        if (dy == 1) return Direction::DOWN;
        if (dy == -1) return Direction::UP;
        return Direction::UP;
    }
}

void Enemy::add(int x, int y) {
    if (enemies.size() < GameConfig::MAX_ENEMIES) {
        enemies.emplace_back(x, y);
    }
}

void Enemy::clearAll() {
    enemies.clear();
}

void Enemy::fillRandom() {
    while (enemies.size() < GameConfig::MAX_ENEMIES) {
        int x = rand() % GameConfig::MAZE_WIDTH;
        int y = rand() % GameConfig::MAZE_HEIGHT;
        if (Maze::isWalkable(x, y)) {
            bool occupied = false;
            for (auto& e : enemies) {
                if (e.getX() == x && e.getY() == y) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) {
                add(x, y);
            }
        }
    }
}

void Enemy::updateAll() {
    Uint64 now = SDL_GetTicks();
    if (now - lastMoveTime < GameConfig::ENEMY_MOVE_INTERVAL) return;

    for (auto& enemy : enemies) {
        int dx = 0, dy = 0;

        switch (enemy.direction) {
        case Direction::RIGHT: dx = 1; break;
        case Direction::DOWN: dy = 1; break;
        case Direction::LEFT: dx = -1; break;
        case Direction::UP: dy = -1; break;
        }

        int newX = enemy.getX() + dx;
        int newY = enemy.getY() + dy;

        if (!Maze::isWalkable(newX, newY)) {
            // Choose a new random direction
            enemy.direction = static_cast<Direction>(rand() % 4);
        }
        else {
            // Move and update facing direction
            enemy.setPosition(newX, newY);
            enemy.setDirection(enemy.direction);
        }
    }

    lastMoveTime = now;
}

void Enemy::renderAll() {
    for (const auto& enemy : enemies) {
        enemy.render();
    }
}

bool Enemy::checkCollisionWithPlayer() {
    int px = Player::getX();
    int py = Player::getY();

    for (const auto& enemy : enemies) {
        if (enemy.getX() == px && enemy.getY() == py) {
            return true;
        }
    }

    return false;
}
