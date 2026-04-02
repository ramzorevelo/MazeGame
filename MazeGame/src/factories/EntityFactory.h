// EntityFactory.h
#pragma once
#include "EntityID.h"
#include <vector>

class EntityRegistry;

namespace EntityFactory {
    // Create a single entity of each type
    EntityID createPlayer(EntityRegistry& reg, int x, int y);
    EntityID createEnemy(EntityRegistry& reg, int x, int y);
    EntityID createItem(EntityRegistry& reg, int x, int y);
    EntityID createGoal(EntityRegistry& reg, int x, int y);

    // Scan layout, create all entities, and random-fill missing enemies/items.
    // outPlayerID is set to the created player's EntityID.
    void createAll(EntityRegistry& reg,
        const std::vector<std::vector<int>>& layout,
        EntityID& outPlayerID);
}