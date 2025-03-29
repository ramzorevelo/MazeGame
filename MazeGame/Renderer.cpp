// Renderer.cpp
#include "Renderer.h"
#include "Game.h"

#include <SDL3_ttf/SDL_ttf.h>

void Renderer::renderText(const std::string& text, int x, int y, SDL_Color color) {
    TTF_Font* font = Game::getFont();
    SDL_Renderer* renderer = Game::getRenderer();

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), text.length(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect dstRect = {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(surface->w),
        static_cast<float>(surface->h)
    };

    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}