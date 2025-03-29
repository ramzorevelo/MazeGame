// VisualEffect.cpp
#include "VisualEffect.h"
#include "GameConfig.h"
#include "Game.h"
#include "Renderer.h"

#include <SDL3/SDL.h>
#include <vector>

namespace {
    struct Effect {
        std::string text = "";
        float x = 0.0f;
        float y = 0.0f;
        float alpha = 255.0f;
        VisualEffect::EffectConfig config;
        Uint64 startTime = 0;
    };

    std::vector<Effect> effects;
}

void VisualEffect::add(const std::string& text, int tileX, int tileY, const EffectConfig& config) {
    int pixelX = tileX * GameConfig::TILE_SIZE + GameConfig::TILE_SIZE / 4;
    int pixelY = tileY * GameConfig::TILE_SIZE + GameConfig::UI_OFFSET_Y;

    Effect fx;
    fx.text = text;
    fx.x = static_cast<float>(pixelX);
    fx.y = static_cast<float>(pixelY);
    fx.config = config;
    fx.startTime = SDL_GetTicks();

    effects.push_back(fx);
}

void VisualEffect::updateAll() {
    Uint64 now = SDL_GetTicks();

    for (size_t i = 0; i < effects.size(); ) {
        Effect& fx = effects[i];
        Uint64 elapsed = now - fx.startTime;

        if (elapsed >= fx.config.duration) {
            effects.erase(effects.begin() + i);
        }
        else {
            fx.y -= fx.config.riseSpeed;
            fx.alpha = 255 * (1.0f - static_cast<float>(elapsed) / fx.config.duration);
            ++i;
        }
    }
}

void VisualEffect::renderAll() {
    for (const auto& fx : effects) {
        SDL_Color color = fx.config.color;
        color.a = static_cast<Uint8>(fx.alpha);

        // NOTE: Renderer currently uses global font size. You may extend it to support per-size fonts.
        Renderer::renderText(fx.text, static_cast<int>(fx.x), static_cast<int>(fx.y), color);
    }
}