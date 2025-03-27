#include "game.h"
#include <SDL3/SDL.h> // For SDL_GetTicks64
#include <vector>


void initializeEnemies() {
    // Do not clear enemies here; preserve enemies from the layout
    // Add more enemies at random path locations until MAX_ENEMIES is reached
    while (enemies.size() < MAX_ENEMIES) {
        // Generate random coordinates
        int x = rand() % MAZE_WIDTH;
        int y = rand() % MAZE_HEIGHT;

        // Check if the position is a PATH and not already occupied by an enemy, item, player, or goal
        if (maze[y][x] == PATH) {
            bool positionOccupied = false;
            // Check if occupied by an enemy
            for (const Enemy& existingEnemy : enemies) {
                if (existingEnemy.x == x && existingEnemy.y == y) {
                    positionOccupied = true;
                    break;
                }
            }
            // Check if occupied by an item
            for (const Item& existingItem : items) {
                if (existingItem.x == x && existingItem.y == y) {
                    positionOccupied = true;
                    break;
                }
            }
            // Check if occupied by player or goal
            if ((x == playerX && y == playerY) || (x == goalX && y == goalY)) {
                positionOccupied = true;
            }

            // If the position is not occupied, add a new enemy
            if (!positionOccupied) {
                Enemy newEnemy = { x, y, rand() % 4 }; // Random direction
                enemies.push_back(newEnemy);
            }
        }
    }
}

void moveEnemies() {
    // Use a timer to control enemy movement speed
    static Uint64 lastEnemyMoveTime = SDL_GetTicks(); // Track the last time enemies moved
    Uint64 currentTime = SDL_GetTicks();
    const Uint64 enemyMoveInterval = 500; // Move enemies every 500ms (same as items, adjust as needed)

    // Only move enemies if enough time has passed
    if (currentTime - lastEnemyMoveTime >= enemyMoveInterval) {
        // Vector to store indices of enemies to remove (if needed, e.g., for future features)
        std::vector<size_t> enemiesToRemove;

        // Iterate over enemies and check for collisions before moving
        for (size_t i = 0; i < enemies.size(); ++i) {
            Enemy& enemy = enemies[i];

            // Check for collision with player at the enemy's current position
            if (enemy.x == playerX && enemy.y == playerY) {
                if (!isInvulnerable) {
                    playerLives--; // Only reduce lives if not invulnerable
                    isInvulnerable = true;
                    invulnerableStartTime = SDL_GetTicks(); // Start timer
                }
                continue; // Skip moving this enemy
            }

            // If not colliding, calculate new position based on direction
            int newX = enemy.x;
            int newY = enemy.y;

            if (enemy.direction == PATH_RIGHT) {
                newX++;
            }
            else if (enemy.direction == PATH_DOWN) {
                newY++;
            }
            else if (enemy.direction == PATH_LEFT) {
                newX--;
            }
            else if (enemy.direction == PATH_UP) {
                newY--;
            }

            // Check if the new position is valid (not a wall, within bounds)
            if (!isValidMove(newX, newY)) {
                // If blocked, choose a new direction that's valid
                std::vector<int> possibleDirections;

                for (int dir = 0; dir < 4; ++dir) {
                    int testX = enemy.x;
                    int testY = enemy.y;

                    if (dir == PATH_RIGHT) testX++;
                    else if (dir == PATH_DOWN) testY++;
                    else if (dir == PATH_LEFT) testX--;
                    else if (dir == PATH_UP) testY--;

                    if (isValidMove(testX, testY)) {
                        possibleDirections.push_back(dir);
                    }
                }

                if (!possibleDirections.empty()) {
                    enemy.direction = possibleDirections[rand() % possibleDirections.size()];
                }
            }
            else {
                // Continue moving in the current direction
                enemy.x = newX;
                enemy.y = newY;
            }
        }

        // Remove enemies marked for removal (if any, currently unused)
        for (auto it = enemiesToRemove.rbegin(); it != enemiesToRemove.rend(); ++it) {
            size_t index = *it;
            enemies.erase(enemies.begin() + index);
        }
        // Update the last move time
        lastEnemyMoveTime = currentTime;
    }
}

void initializeMaze(const std::vector<std::vector<int>>& layout) {
    // First, check if the provided layout is valid (dimensions match)
    if (layout.empty() || layout.size() != MAZE_HEIGHT) {
        SDL_Log("Error: Invalid maze layout height.");
        return;
    }
    for (const auto& row : layout) {
        if (row.size() != MAZE_WIDTH) {
            SDL_Log("Error: Invalid maze layout width.");
            return;
        }
    }

    // Clear the items and enemies vectors to start fresh (before processing layout)
    items.clear();
    enemies.clear();

    // 1. Initialize the maze array AND create Item and Enemy structs from the layout
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            maze[y][x] = layout[y][x]; // Set the maze value

            if (layout[y][x] == PLAYER_INT) {
                playerX = x;
                playerY = y;
                maze[y][x] = PATH;
            }
            else if (layout[y][x] == GOAL_INT) {
                goalX = x;
                goalY = y;
                maze[y][x] = PATH;
            }
            else if (layout[y][x] == ITEM) {
                // Create an Item struct and add it to the vector
                if (items.size() < MAX_ITEMS) { // Check for max items
                    Item newItem = { x, y, rand() % 4 }; // Random initial direction
                    items.push_back(newItem);
                }
                maze[y][x] = PATH; // Replace item with path in the maze
            }
            else if (layout[y][x] == ENEMY) { // New: Process enemies from the layout
                // Create an Enemy struct and add it to the vector
                if (enemies.size() < MAX_ENEMIES) { // Check for max enemies
                    std::vector<int> validDirections;
                    for (int dir = 0; dir < 4; ++dir) {
                        int testX = x;
                        int testY = y;

                        if (dir == PATH_RIGHT) testX++;
                        else if (dir == PATH_DOWN) testY++;
                        else if (dir == PATH_LEFT) testX--;
                        else if (dir == PATH_UP) testY--;

                        // Check if valid and NOT pointing toward the player
                        if (isValidMove(testX, testY)) {
                            // Avoid spawning facing the player
                            if (!(testX == playerX && testY == playerY)) {
                                validDirections.push_back(dir);
                            }
                        }
                    }

                    if (!validDirections.empty()) {
                        Enemy newEnemy = { x, y, validDirections[rand() % validDirections.size()] };
                        enemies.push_back(newEnemy);
                    }
                }
                maze[y][x] = PATH; // Replace enemy with path in the maze
            }
        }
    }

    // Call initializeItems and initializeEnemies to fill remaining slots
    initializeItems();
    initializeEnemies();

    // Reset score, health, and player lives
    score = 0;
    health = 100;
    playerLives = 3; // Start with 3 lives
}

bool checkLose() {
    // Use a timer to decrease health at a controlled rate (e.g., every second)
    static Uint64 lastHealthDecreaseTime = SDL_GetTicks(); // Get current time in milliseconds
    Uint64 currentTime = SDL_GetTicks();
    const Uint64 healthDecreaseInterval = 1000; // Decrease health every 1000 ms (1 second)

    if (currentTime - lastHealthDecreaseTime >= healthDecreaseInterval) {
        health -= 1; // Decrease health over time
        lastHealthDecreaseTime = currentTime; // Reset timer
    }

    // Check if health is depleted or player lives are depleted
    if (health <= 0 || playerLives <= 0) {
        gameOver = true;
        gameWon = false;
        return true;
    }
    return false;
}

void updateGameState() {
    // Check win condition
    if (checkWin()) {
        return;
    }
    moveItems(); // Move items *before* checking for loss
    moveEnemies(); // Move enemies *before* checking for loss
    // Handle invulnerability timeout
    if (isInvulnerable) {
        Uint64 currentTime = SDL_GetTicks();
        if (currentTime - invulnerableStartTime >= 1000) { // 1000 ms = 1s
            isInvulnerable = false;
        }
    }
    // Check lose condition
    if (checkLose()) {
        return;
    }
    // After all movement, check again if player is on an item
    for (size_t i = 0; i < items.size(); ) {
        if (items[i].x == playerX && items[i].y == playerY) {
            collectItem(items[i].x, items[i].y);
            items.erase(items.begin() + i);
        }
        else {
            ++i;
        }
    }
}
void setPathRight(int startX, int startY, int length) {
    // Ensure starting point is within bounds
    if (startY < 0 || startY >= MAZE_HEIGHT || startX < 0 || startX >= MAZE_WIDTH) {
        return; // Invalid starting point
    }

    // Set paths to the right, but stop at the right edge
    for (int i = 0; i < length; i++) {
        int x = startX + i;
        if (x >= MAZE_WIDTH) {
            break; // Stop if we go out of bounds
        }
        maze[startY][x] = PATH; // TODO: Students fill in this line
    }
}

void setPathDown(int startX, int startY, int length) {
    // Ensure starting point is within bounds
    if (startY < 0 || startY >= MAZE_HEIGHT || startX < 0 || startX >= MAZE_WIDTH) {
        return; // Invalid starting point
    }

    // Set paths downward, but stop at the bottom edge
    for (int i = 0; i < length; i++) {
        int y = startY + i;
        if (y >= MAZE_HEIGHT) {
            break; // Stop if we go out of bounds
        }
        maze[y][startX] = PATH; // TODO: Students fill in this line
    }
}

void setPath(int startX, int startY, int length, int direction) {
    // Ensure starting point is within bounds
    if (startY < 0 || startY >= MAZE_HEIGHT || startX < 0 || startX >= MAZE_WIDTH) {
        return; // Invalid starting point
    }

    // Set paths in the specified direction, with bounds checking
    for (int i = 0; i < length; i++) {
        int x = startX;
        int y = startY;

        // Adjust coordinates based on direction
        if (direction == PATH_RIGHT) {
            x += i;
        }
        else if (direction == PATH_DOWN) {
            y += i;
        }
        else if (direction == PATH_LEFT) {
            x -= i;
        }
        else if (direction == PATH_UP) {
            y -= i;
        }
        else {
            return; // Invalid direction
        }

        // Check bounds
        if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) {
            break; // Stop if we go out of bounds
        }

        maze[y][x] = PATH; // TODO: Students fill in this line
    }
}

void setItem(int x, int y) {
    // Ensure coordinates are within bounds
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) {
        return; // Invalid coordinates
    }

    maze[y][x] = ITEM; // TODO: Students fill in this line
}

void setPlayer(int x, int y) {
    // Ensure coordinates are within bounds
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) {
        return; // Invalid coordinates
    }

    // Ensure the player is placed on a path
    if (maze[y][x] != PATH) {
        maze[y][x] = PATH; // Ensure the spot is a path
    }

    playerX = x; // TODO: Students fill in this line
    playerY = y; // TODO: Students fill in this line
}

void setGoal(int x, int y) {
    // Ensure coordinates are within bounds
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) {
        return; // Invalid coordinates
    }

    // Ensure the goal is placed on a path
    if (maze[y][x] != PATH) {
        maze[y][x] = PATH; // Ensure the spot is a path
    }

    goalX = x; // TODO: Students fill in this line
    goalY = y; // TODO: Students fill in this line
}

void initializeItems() {
    // Do not clear items here; preserve items from the layout
    // Add more items at random path locations until MAX_ITEMS is reached
    while (items.size() < MAX_ITEMS) {
        // Generate random coordinates
        int x = rand() % MAZE_WIDTH;
        int y = rand() % MAZE_HEIGHT;

        // Check if the position is a PATH and not already occupied by an item
        if (maze[y][x] == PATH) {
            bool positionOccupied = false;
            for (const Item& existingItem : items) {
                if (existingItem.x == x && existingItem.y == y) {
                    positionOccupied = true;
                    break;
                }
            }

            // If the position is not occupied, add a new item
            if (!positionOccupied) {
                Item newItem = { x, y, rand() % 4 }; // Random direction
                items.push_back(newItem);
            }
        }
    }
}

bool isValidMove(int x, int y) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT || maze[y][x] == WALL) {
        return false;
    }
    return true;
}

void moveItems() {
    // Use a timer to control item movement speed
    static Uint64 lastItemMoveTime = SDL_GetTicks(); // Track the last time items moved
    Uint64 currentTime = SDL_GetTicks();
    const Uint64 itemMoveInterval = 500; // Move items every 500ms (adjust as needed)

    // Only move items if enough time has passed
    if (currentTime - lastItemMoveTime >= itemMoveInterval) {
        // Vector to store indices of items to remove
        std::vector<size_t> itemsToRemove;

        // Iterate over items and check for collisions before moving
        for (size_t i = 0; i < items.size(); ++i) {
            Item& item = items[i];

            // Check for collision with player at the item's current position
            if (item.x == playerX && item.y == playerY) {
                collectItem(item.x, item.y); // Collect the item
                itemsToRemove.push_back(i); // Mark this item for removal
                continue; // Skip moving this item since it was collected
            }

            // If not collected, calculate new position based on direction
            int newX = item.x;
            int newY = item.y;

            if (item.direction == PATH_RIGHT) {
                newX++;
            }
            else if (item.direction == PATH_DOWN) {
                newY++;
            }
            else if (item.direction == PATH_LEFT) {
                newX--;
            }
            else if (item.direction == PATH_UP) {
                newY--;
            }

            // Check if the new position is valid (not a wall, within bounds)
            if (!isValidMove(newX, newY)) {
                // If invalid, change direction randomly
                item.direction = rand() % 4;
            }
            else {
                // If valid, update item position
                item.x = newX;
                item.y = newY;
            }
        }

        // Remove collected items in reverse order to avoid index shifting issues
        for (auto it = itemsToRemove.rbegin(); it != itemsToRemove.rend(); ++it) {
            size_t index = *it;
            items.erase(items.begin() + index);
        }

        // Update the last move time
        lastItemMoveTime = currentTime;
    }
}
bool movePlayer(int newX, int newY) {
    // Check if the new position is within maze bounds
    if (newX < 0 || newX >= MAZE_WIDTH || newY < 0 || newY >= MAZE_HEIGHT) {
        return false; // TODO: Students fill in this line
    }

    // Check if the new position is a wall
    if (maze[newY][newX] == WALL) {
        return false; // TODO: Students fill in this line
    }

    // If the new position has an item, collect it
    if (maze[newY][newX] == ITEM) {
        collectItem(newX, newY); // TODO: Students fill in this line
    }

    // Update player position
    playerX = newX; // TODO: Students fill in this line
    playerY = newY; // TODO: Students fill in this line

    return true;
}

void collectItem(int x, int y) {
    // Increase score
    score += 10;

    // Remove the item from the maze
    maze[y][x] = PATH;

    // Add a visual effect
    int yOffset = 100; // Same offset as maze rendering
    VisualEffect effect;
    effect.text = "+10";
    effect.x = static_cast<float>(x * TILE_SIZE + TILE_SIZE / 4);
    effect.y = static_cast<float>(y * TILE_SIZE + yOffset);
    effect.startTime = SDL_GetTicks();
    effect.duration = 1000; // 1 second
    effect.alpha = 255;
    effect.riseSpeed = 0.05f;

    visualEffects.push_back(effect);
}

bool checkWin() {
    // Check if player has reached the goal
    if (playerX == goalX && playerY == goalY) {
        gameOver = true; // TODO: Students fill in this line
        gameWon = true;  // TODO: Students fill in this line
        return true;     // TODO: Students fill in this line
    }
    return false;
}

