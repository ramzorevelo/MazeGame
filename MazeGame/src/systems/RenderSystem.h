// RenderSystem.h
#pragma once
#include <SDL3/SDL.h>

class EntityRegistry;

class RenderSystem {
public:
    void render(EntityRegistry& reg, SDL_Renderer* renderer);
};