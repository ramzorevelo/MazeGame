// Renderer.h
#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace Renderer {
    // Render simple solid text at x, y
    void renderText(const std::string& text, int x, int y, SDL_Color color);
}