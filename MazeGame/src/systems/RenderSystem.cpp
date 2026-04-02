// RenderSystem.cpp
#include "RenderSystem.h"
#include "EntityRegistry.h"
#include "Components.h"
#include "GameConfig.h"
#include "ShapeRenderer.h"
#include <SDL3/SDL.h>

void RenderSystem::render(EntityRegistry& reg, SDL_Renderer* renderer) {
    reg.forEach<PositionComponent, RenderComponent>(
        [&](EntityID id, PositionComponent& pos, RenderComponent& rc) {
            // Blink when invulnerable (applies to player entity)
            if (reg.hasComponent<HealthComponent>(id)) {
                const auto& hp{ reg.getComponent<HealthComponent>(id) };
                if (hp.invulnerable && (SDL_GetTicks() / 100ULL) % 2ULL == 0ULL)
                    return; // skip this frame
            }

            const int cx{ pos.x * GameConfig::TILE_SIZE + GameConfig::TILE_SIZE / 2 };
            const int cy{ pos.y * GameConfig::TILE_SIZE
                + GameConfig::TILE_SIZE / 2
                + GameConfig::UI_OFFSET_Y };
            const int size{ GameConfig::TILE_SIZE / 2 - 4 };

            ShapeRenderer::drawShape(renderer, rc.shape, cx, cy, size,
                rc.fill, rc.outline, rc.rotation);
        }
    );
}