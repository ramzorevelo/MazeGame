// Maze.cpp
#include "Maze.h"
#include "GameConfig.h"
#include "Renderer.h"

#include "Player.h"
#include "Goal.h"
#include "Item.h"
#include "Enemy.h"

#include <SDL3/SDL.h>
#include "Game.h"

// This file handles the maze grid and initial placement of everything

enum TileType {
    WALL = 0,
    PATH = 1,
    ITEM = 2,
    PLAYER_INT = 3,
    GOAL_INT = 4,
    ENEMY_INT = 5
};

namespace {
    int maze[GameConfig::MAZE_HEIGHT][GameConfig::MAZE_WIDTH];
    std::vector<std::vector<int>> originalLayout;
}

void Maze::loadLayout(const std::vector<std::vector<int>>& layout) {
    // Save original for restart
    originalLayout = layout;

    // Clear old items and enemies
    Item::clearAll();
    Enemy::clearAll();

    // Place everything based on tile values
    for (int y = 0; y < GameConfig::MAZE_HEIGHT; ++y) {
        for (int x = 0; x < GameConfig::MAZE_WIDTH; ++x) {
            int cell = layout[y][x];

            switch (cell) {
            case WALL: maze[y][x] = WALL; break;
            case PATH: maze[y][x] = PATH; break;
            case ITEM:
                maze[y][x] = PATH;
                Item::add(x, y);
                break;
            case PLAYER_INT:
                maze[y][x] = PATH;
                Player::setPosition(x, y);
                break;
            case GOAL_INT:
                maze[y][x] = PATH;
                Goal::setPosition(x, y);
                break;
            case ENEMY_INT:
                maze[y][x] = PATH;
                Enemy::add(x, y);
                break;
            default:
                maze[y][x] = WALL;
            }
        }
    }

    // Randomly fill missing items and enemies
    Item::fillRandom();
    Enemy::fillRandom();
}

void Maze::reload() {
    loadLayout(originalLayout);
}

bool Maze::isWalkable(int x, int y) {
    if (x < 0 || x >= GameConfig::MAZE_WIDTH || y < 0 || y >= GameConfig::MAZE_HEIGHT) {
        return false;
    }
    return maze[y][x] != WALL;
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
            SDL_Color color = (maze[y][x] == WALL) ? GameConfig::COLOR_WALL : GameConfig::COLOR_PATH;
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &tileRect);
        }
    }
}