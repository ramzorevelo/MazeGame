// AISystem.cpp
#include "AISystem.h"
#include "EntityRegistry.h"
#include "Components.h"
#include "Maze.h"
#include "Direction.h"
#include <SDL3/SDL.h>
#include <cstdlib>

void AISystem::update(EntityRegistry& reg) {
    Uint64 now{ SDL_GetTicks() };

    reg.forEach<PositionComponent, AIComponent, MovementComponent, RenderComponent>(
        [&](EntityID /*id*/,
            PositionComponent& pos,
            AIComponent& ai,
            MovementComponent& mv,
            RenderComponent& rc)
        {
            if (now - mv.lastMoveTime < mv.moveInterval) return;

            int dx{ 0 };
            int dy{ 0 };
            switch (ai.direction) {
            case Direction::UP:    dy = -1; break;
            case Direction::DOWN:  dy = 1; break;
            case Direction::LEFT:  dx = -1; break;
            case Direction::RIGHT: dx = 1; break;
            }

            int nx{ pos.x + dx };
            int ny{ pos.y + dy };

            if (!Maze::isWalkable(nx, ny)) {
                // Bounce: choose a new random direction
                ai.direction = static_cast<Direction>(rand() % 4);
            }
            else {
                pos.x = nx;
                pos.y = ny;
                if (rc.rotEnabled)
                    rc.rotation = directionToAngle(ai.direction);
            }

            mv.lastMoveTime = now;
        }
    );
}