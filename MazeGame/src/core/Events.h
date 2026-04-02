// Events.h
#pragma once
#include "EntityID.h"

// Published by InputSystem when a bound movement key is pressed
struct MoveActionEvent {
    int dx{ 0 };
    int dy{ 0 };
};

// Published by CollisionSystem when an enemy occupies the player's tile
// (lives are already decremented on the HealthComponent before publishing)
struct EnemyHitEvent {};

// Published by CollisionSystem when the player steps on an item
struct ItemCollectedEvent {
    EntityID itemID;
    int tileX{ 0 };
    int tileY{ 0 };
    int points{ 0 };
};

// Published by CollisionSystem when the player reaches the goal
struct GoalReachedEvent {};