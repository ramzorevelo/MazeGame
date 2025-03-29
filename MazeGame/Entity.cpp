// Entity.cpp
#include "Entity.h"
#include "GameConfig.h"
#include "Game.h"

Entity::Entity(int x, int y, ShapeType shape, SDL_Color fill, SDL_Color outline)
    : x(x), y(y), shape(shape), fillColor(fill), outlineColor(outline) {
}

void Entity::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void Entity::setDirection(Direction dir) {
    facing = dir;
}

Direction Entity::getDirection() const {
    return facing;
}

void Entity::enableRotation(bool enable) {
    rotateWithDirection = enable;
}

void Entity::render() const {
    // Convert tile coordinates to pixel center
    int centerX = x * GameConfig::TILE_SIZE + GameConfig::TILE_SIZE / 2;
    int centerY = y * GameConfig::TILE_SIZE + GameConfig::TILE_SIZE / 2 + GameConfig::UI_OFFSET_Y;
    int size = GameConfig::TILE_SIZE / 3;


    float angle = rotateWithDirection ? directionToAngle(facing) : 0.0f;

    // Draw shape with outline and fill
    ShapeRenderer::drawShape(Game::getRenderer(), shape, centerX, centerY, size, fillColor, outlineColor, angle);
}