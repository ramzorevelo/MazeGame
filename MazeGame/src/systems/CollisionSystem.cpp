// CollisionSystem.cpp
#include "CollisionSystem.h"
#include "EntityRegistry.h"
#include "EventBus.h"
#include "Events.h"
#include "Components.h"
#include "GameConfig.h"
#include <vector>
#include <utility>
#include <SDL3/SDL.h>

void CollisionSystem::updateInvulnerability(EntityRegistry& reg) {
    Uint64 now{ SDL_GetTicks() };
    reg.forEach<HealthComponent>(
        [&](EntityID /*id*/, HealthComponent& hp) {
            if (hp.invulnerable &&
                now - hp.invulnerableStart >= hp.invulnerableDuration)
                hp.invulnerable = false;
        }
    );
}

void CollisionSystem::update(EntityRegistry& reg, EventBus& bus) {
    // --- Locate player ---
    EntityID          playerID{ NULL_ENTITY };
    PositionComponent* playerPos{ nullptr };
    HealthComponent* playerHP{ nullptr };

    reg.forEach<PlayerTag, PositionComponent, HealthComponent>(
        [&](EntityID id, PlayerTag&, PositionComponent& pos, HealthComponent& hp) {
            playerID = id;
            playerPos = &pos;
            playerHP = &hp;
        }
    );
    if (playerID == NULL_ENTITY) return;

    const int px{ playerPos->x };
    const int py{ playerPos->y };

    // --- Player vs Enemies ---
    if (!playerHP->invulnerable) {
        reg.forEach<EnemyTag, PositionComponent>(
            [&](EntityID /*id*/, EnemyTag&, PositionComponent& epos) {
                if (epos.x == px && epos.y == py) {
                    playerHP->lives -= GameConfig::LIFE_LOSS_ON_HIT;
                    playerHP->invulnerable = true;
                    playerHP->invulnerableStart = SDL_GetTicks();
                    bus.publish(EnemyHitEvent{});
                }
            }
        );
    }

    // --- Player vs Items ---
    // Collect positions before destroying to avoid dangling refs
    std::vector<std::pair<EntityID, PositionComponent>> collected;
    reg.forEach<ItemTag, PositionComponent>(
        [&](EntityID id, ItemTag&, PositionComponent& ipos) {
            if (ipos.x == px && ipos.y == py)
                collected.emplace_back(id, ipos);
        }
    );
    for (auto& [id, ipos] : collected) {
        bus.publish(ItemCollectedEvent{ id, ipos.x, ipos.y, GameConfig::ITEM_SCORE });
        reg.destroyEntity(id);
    }

    // --- Player vs Goal ---
    reg.forEach<GoalTag, PositionComponent>(
        [&](EntityID /*id*/, GoalTag&, PositionComponent& gpos) {
            if (gpos.x == px && gpos.y == py)
                bus.publish(GoalReachedEvent{});
        }
    );
}