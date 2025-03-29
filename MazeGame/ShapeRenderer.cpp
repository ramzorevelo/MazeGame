// ShapeRenderer.cpp
#include "ShapeRenderer.h"
#include <cmath>

// Convert SDL_Color to SDL_FColor
SDL_FColor toFColor(SDL_Color c) {
    return SDL_FColor{
        static_cast<float>(c.r) / 255.0f,
        static_cast<float>(c.g) / 255.0f,
        static_cast<float>(c.b) / 255.0f,
        static_cast<float>(c.a) / 255.0f
    };
}

// Rotate a point around a center
SDL_FPoint rotatePoint(SDL_FPoint pt, SDL_FPoint center, float angleDegrees) {
    float angleRad = angleDegrees * (3.14159265f / 180.0f);
    float s = sinf(angleRad);
    float c = cosf(angleRad);

    pt.x -= center.x;
    pt.y -= center.y;

    float xnew = pt.x * c - pt.y * s;
    float ynew = pt.x * s + pt.y * c;

    pt.x = xnew + center.x;
    pt.y = ynew + center.y;

    return pt;
}

// Draw filled circle (no rotation needed)
void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = -radius; w <= radius; ++w) {
        for (int h = -radius; h <= radius; ++h) {
            if (w * w + h * h <= radius * radius) {
                SDL_RenderPoint(renderer, static_cast<float>(centerX + w), static_cast<float>(centerY + h));
            }
        }
    }
}

// Draw filled triangle with rotation
void drawFilledTriangle(SDL_Renderer* renderer, int cx, int cy, int size, SDL_Color color, float angle) {
    SDL_FPoint center = { static_cast<float>(cx), static_cast<float>(cy) };
    SDL_FPoint p1 = rotatePoint({ static_cast<float>(cx), static_cast<float>(cy - size) }, center, angle);         // top
    SDL_FPoint p2 = rotatePoint({ static_cast<float>(cx - size), static_cast<float>(cy + size) }, center, angle);  // bottom-left
    SDL_FPoint p3 = rotatePoint({ static_cast<float>(cx + size), static_cast<float>(cy + size) }, center, angle);  // bottom-right

    SDL_Vertex verts[3] = {
        { p1, toFColor(color), { 0.0f, 0.0f } },
        { p2, toFColor(color), { 0.0f, 0.0f } },
        { p3, toFColor(color), { 0.0f, 0.0f } }
    };

    SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
}

// Draw filled diamond with rotation
void drawFilledDiamond(SDL_Renderer* renderer, int cx, int cy, int size, SDL_Color color, float angle) {
    SDL_FPoint center = { static_cast<float>(cx), static_cast<float>(cy) };

    SDL_FPoint points[4] = {
        rotatePoint({ static_cast<float>(cx), static_cast<float>(cy - size) }, center, angle),
        rotatePoint({ static_cast<float>(cx + size), static_cast<float>(cy) }, center, angle),
        rotatePoint({ static_cast<float>(cx), static_cast<float>(cy + size) }, center, angle),
        rotatePoint({ static_cast<float>(cx - size), static_cast<float>(cy) }, center, angle)
    };

    SDL_Vertex verts[6] = {
        { points[0], toFColor(color), { 0.0f, 0.0f } },
        { points[1], toFColor(color), { 0.0f, 0.0f } },
        { points[2], toFColor(color), { 0.0f, 0.0f } },

        { points[2], toFColor(color), { 0.0f, 0.0f } },
        { points[3], toFColor(color), { 0.0f, 0.0f } },
        { points[0], toFColor(color), { 0.0f, 0.0f } },
    };

    SDL_RenderGeometry(renderer, nullptr, verts, 6, nullptr, 0);
}

void ShapeRenderer::drawShape(SDL_Renderer* renderer,
    ShapeType shape,
    int centerX, int centerY,
    int size,
    SDL_Color fill,
    SDL_Color outline,
    float rotationDegrees) {

    int outlineSize = size + 2;

    switch (shape) {
    case ShapeType::CIRCLE:
        drawFilledCircle(renderer, centerX, centerY, outlineSize, outline);
        drawFilledCircle(renderer, centerX, centerY, size, fill);
        break;

    case ShapeType::TRIANGLE:
        drawFilledTriangle(renderer, centerX, centerY, outlineSize, outline, rotationDegrees);
        drawFilledTriangle(renderer, centerX, centerY, size, fill, rotationDegrees);
        break;

    case ShapeType::DIAMOND:
        drawFilledDiamond(renderer, centerX, centerY, outlineSize, outline, rotationDegrees);
        drawFilledDiamond(renderer, centerX, centerY, size, fill, rotationDegrees);
        break;
    }
}