// Maze.h
#pragma once

#include <vector>

namespace Maze {
    // Load the initial layout from a 2D vector (called once at start)
    void loadLayout(const std::vector<std::vector<int>>& layout);

    // Reload the same layout (used for restart)
    void reload();

    // Render the maze tiles (walls and paths only)
    void render();

    // Check if a tile is walkable (not a wall)
    bool isWalkable(int x, int y);
}