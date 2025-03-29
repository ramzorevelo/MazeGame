// Enemy.h
#pragma once
#include "Entity.h"
#include <vector>

namespace Enemy {
    void add(int x, int y);
    void clearAll();
    void fillRandom();

    void updateAll();
    void renderAll();

    // Check if any enemy is at the player's position
    bool checkCollisionWithPlayer();
}