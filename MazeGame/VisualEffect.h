// VisualEffect.h
#pragma once
#include <string>
#include <SDL3/SDL.h>

namespace VisualEffect {

    struct EffectConfig {
        SDL_Color color = { 255, 255, 0, 255 }; // default yellow
        float riseSpeed = 0.05f;
        Uint64 duration = 1000; // in milliseconds
        int fontSize = 24;      // default font size
    };

    // Add a new floating text effect with optional config
    void add(const std::string& text, int tileX, int tileY, const EffectConfig& config = EffectConfig());

    void updateAll();
    void renderAll();
}