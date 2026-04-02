// CollisionSystem.h
#pragma once

class EntityRegistry;
class EventBus;

class CollisionSystem {
public:
    // Expire invulnerability windows whose timer has elapsed
    void updateInvulnerability(EntityRegistry& reg);

    // Detect player vs enemy / item / goal; publish events; modify components
    void update(EntityRegistry& reg, EventBus& bus);
};