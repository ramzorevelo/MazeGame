// AISystem.h
// Handles timed movement for entities with AIComponent (enemies, items).
#pragma once

class EntityRegistry;

class AISystem {
public:
    void update(EntityRegistry& reg);
};