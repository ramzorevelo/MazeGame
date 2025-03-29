// Entity.h
#pragma once
#include "ShapeRenderer.h"
#include "Direction.h"
#include <SDL3/SDL.h>

// This class represents any object that appears on the maze (player, enemy, item, goal)
class Entity {
public:
    // Constructor: set position, shape, and colors
    Entity(int x, int y, ShapeType shape, SDL_Color fill, SDL_Color outline);
    virtual ~Entity() = default;

    // Draw the entity on screen
    virtual void render() const;

    // Update logic (can be overridden by child classes)
    virtual void update() {}

    // Get current tile position
    int getX() const { return x; }
    int getY() const { return y; }

    // Change position
    void setPosition(int newX, int newY);
    void setDirection(Direction dir);
    Direction getDirection() const;

    void enableRotation(bool enable);

protected:
    int x, y;                   // Grid position
    ShapeType shape;            // Circle, triangle, diamond
    SDL_Color fillColor;        // Inside color
    SDL_Color outlineColor;     // Border color

    Direction facing = Direction::UP;
    bool rotateWithDirection = false;
};