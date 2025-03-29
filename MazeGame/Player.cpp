// Player.cpp
#include "Player.h"
#include "GameConfig.h"
#include "Maze.h"

namespace {
    // You can change the shape and colors here!
    Entity player(1, 1,
        ShapeType::CIRCLE,
        GameConfig::COLOR_PLAYER_FILL,
        GameConfig::COLOR_PLAYER_OUTLINE);
}

void Player::setPosition(int x, int y) {
    player.setPosition(x, y);
}

void Player::move(int dx, int dy) {
    int newX = player.getX() + dx;
    int newY = player.getY() + dy;

    // Only move if the target tile is walkable
    if (Maze::isWalkable(newX, newY)) {
        player.setPosition(newX, newY);
    }
}

void Player::update() {
    // Currently nothing needed
    // You can add custom logic here!
    // Example: change color if on special tile
}

void Player::render(bool invulnerable) {
    // Blinking effect when invulnerable
    if (!invulnerable || (SDL_GetTicks() / 100) % 2 == 0) {
        player.render();
    }
}

int Player::getX() { return player.getX(); }
int Player::getY() { return player.getY(); }

