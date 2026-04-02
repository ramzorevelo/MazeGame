// Maze.cpp
#include "Maze.h"
#include "GameConfig.h"
#include "Renderer.h"


#include <SDL3/SDL.h>
#include "Game.h"

// This file handles the maze grid and initial placement of everything

enum class TileType : int {
    Wall = 0,
    Path = 1,
    Item = 2,
    Player = 3,
    Goal = 4,
    Enemy = 5
};

namespace {
    TileType maze[GameConfig::MAZE_HEIGHT][GameConfig::MAZE_WIDTH]{};
    std::vector<std::vector<int>> originalLayout{};
}


bool Maze::isWalkable(int x, int y) {
    if (x < 0 || x >= GameConfig::MAZE_WIDTH || y < 0 || y >= GameConfig::MAZE_HEIGHT) {
        return false;
    }
    return maze[y][x] != TileType::Wall;
}

void Maze::render() {
    SDL_Renderer* renderer = Game::getRenderer();

    for (int y = 0; y < GameConfig::MAZE_HEIGHT; ++y) {
        for (int x = 0; x < GameConfig::MAZE_WIDTH; ++x) {
            SDL_FRect tileRect = {
                static_cast<float>(x * GameConfig::TILE_SIZE),
                static_cast<float>(y * GameConfig::TILE_SIZE + GameConfig::UI_OFFSET_Y),
                static_cast<float>(GameConfig::TILE_SIZE),
                static_cast<float>(GameConfig::TILE_SIZE)
            };

            // Custom tile colors
            SDL_Color color = (maze[y][x] == TileType::Wall) ? GameConfig::COLOR_WALL : GameConfig::COLOR_PATH;
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &tileRect);
        }
    }
}

void Maze::initFromLayout(const std::vector<std::vector<int>>& layout) {
    originalLayout = layout;  // already stored in the anonymous namespace
    for (int y{ 0 }; y < GameConfig::MAZE_HEIGHT; ++y)
        for (int x{ 0 }; x < GameConfig::MAZE_WIDTH; ++x)
            maze[y][x] = (layout[y][x] == static_cast<int>(TileType::Wall)) ? TileType::Wall : TileType::Path;
}

const std::vector<std::vector<int>>& Maze::getOriginalLayout() {
    return originalLayout;
}