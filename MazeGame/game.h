#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>

// Constants for maze size (students can change these)
const int MAZE_WIDTH = 20;
const int MAZE_HEIGHT = 15;
const int TILE_SIZE = 64;   // Pixel size of each tile for rendering
const int WINDOW_WIDTH = MAZE_WIDTH * TILE_SIZE;
const int WINDOW_HEIGHT = MAZE_HEIGHT * TILE_SIZE + 100; // Extra height for text display

// Tile types in the maze
const int WALL = 0;
const int PATH = 1;
const int ITEM = 2;
const char PLAYER = 'P';
const char GOAL = 'G';
const int PLAYER_INT = 3;
const int GOAL_INT = 4;
const int ENEMY = 5; // New constant for enemies in the layout
// Path direction constants (for setPath function)
const int PATH_RIGHT = 0;
const int PATH_DOWN = 1;
const int PATH_LEFT = 2;
const int PATH_UP = 3;
const int MAX_ITEMS = 5; // Or however many you want
const int MAX_ENEMIES = 3; // Maximum number of enemies (adjust as needed)

struct Item {
    int x;
    int y;
    int direction; // Use PATH_RIGHT, PATH_DOWN, etc.
};

struct Enemy { // New struct for enemies
    int x;
    int y;
    int direction; // Use PATH_RIGHT, PATH_DOWN, etc.
};
struct VisualEffect {
    std::string text;      // e.g., "+10"
    float x, y;            // Screen position (pixels)
    Uint64 startTime;      // When it appeared
    Uint64 duration;       // How long it should last (ms)
    float alpha;           // Transparency
    float riseSpeed;       // Speed to float upward
};
// Game state
extern int maze[MAZE_HEIGHT][MAZE_WIDTH];
extern int playerX, playerY; // Player position
extern int goalX, goalY;     // Goal position
extern int score;
extern int health;
extern int playerLives; // New variable for player lives
extern bool gameOver;
extern bool gameWon;
extern std::vector<Item> items; // Vector to hold item data
extern std::vector<Enemy> enemies; // New vector to hold enemy data
extern bool isInvulnerable;
extern Uint64 invulnerableStartTime;
extern std::vector<VisualEffect> visualEffects;
// SDL3 globals (students won't modify these)
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font; // Font for text rendering
extern SDL_Color textColor; // Color for text

// Function declarations (game.cpp)
bool initSDL();
void closeSDL();
void renderMaze();
void handleInput(SDL_Event& event);
void renderText(const std::string& text, int x, int y); // New function for text rendering

// Function declarations (game_logic.cpp)
void initializeMaze(const std::vector<std::vector<int>>& layout);
bool movePlayer(int newX, int newY);
void collectItem(int x, int y);
bool checkWin();
bool checkLose();
void updateGameState();
void setPathRight(int startX, int startY, int length);
void setPathDown(int startX, int startY, int length);
void setPath(int startX, int startY, int length, int direction);
void setPlayer(int x, int y);
void setGoal(int x, int y);
void initializeItems();
void moveItems();
void initializeEnemies(); // New function to initialize enemies
void moveEnemies(); // New function to move enemies
bool isValidMove(int x, int y);
#endif // GAME_H