// ShapeRenderer.h
#pragma once
#include <SDL3/SDL.h>

enum class ShapeType {
    CIRCLE,
    TRIANGLE,
    DIAMOND
};

namespace ShapeRenderer {
    void drawShape(
        SDL_Renderer* renderer,
        ShapeType shape,
        int centerX, int centerY,
        int size,
        SDL_Color fill,
        SDL_Color outline,
        float rotationDegrees = 0.0f
    );
}