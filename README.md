# Maze Game (C++ with SDL3)

This is a simple maze game written in C++ using SDL3. It is designed to help you practice and explore C++ programming by working on a real project.

The game features player movement, item collection, enemies, scoring, and win/loss conditions — all implemented in a way that is beginner-friendly and easy to modify.

---

## Requirements

- Visual Studio 2022 (Community or higher)
- Windows 10 or 11
- No additional downloads required — SDL3 and SDL_ttf are already included in the project folder

---

## Getting Started

### 1. Get the Project Files

You have two options:

**Option A: Clone the repository (recommended)**

1. Open Visual Studio 2022
2. Go to **File > Clone Repository**
3. Copy and paste https://github.com/ramzorevelo/MazeGame.git
4. Choose a folder for the project
5. Click **Clone**

**Option B: Download ZIP**

1. Go to the GitHub repository page
2. Click **Code > Download ZIP**
3. Extract the ZIP file to a folder
4. Open the folder

### 2. Open the Project

- In the project folder, double-click `MazeGame.sln` to open it in Visual Studio 2022.
- Set the build configuration to **Debug**
- Set the platform to **x64**

### 3. Build and Run

- Press **Ctrl + F5** to build and run the game (without debugging)
- The game window should open and run immediately

If you see any errors about missing DLLs, make sure the following files are in the same folder as the executable (`Debug` or `Release`):

- `SDL3.dll`
- `SDL3_ttf.dll`
- `font.ttf`

These are already included in the project and should copy automatically.
---

## Controls

- Arrow keys: Move the player
- Collect yellow items to earn points
- Avoid red enemies (you lose lives on contact)
- Reach the blue goal tile to win
- Running out of time or lives ends the game

---

## Project Overview

| File                | Description                             |
|---------------------|-----------------------------------------|
| `main.cpp`          | Entry point, loads the maze layout       |
| `Game.*`            | Game loop, input, update, render         |
| `Maze.*`            | Maze layout and rendering                |
| `Player.*`          | Player logic                             |
| `Enemy.*`           | Enemy movement and logic                 |
| `Item.*`            | Item logic and collection                |
| `Goal.*`            | Goal tile and win condition              |
| `UIManager.*`       | Displays score, time, and lives          |
| `VisualEffect.*`    | Floating text effects                    |
| `ShapeRenderer.*`   | Renders shapes like circle/triangle      |
| `GameConfig.h`      | Main configuration file for the game     |
| `font.ttf`          | Font used for UI text (included)         |

---

## Customization Guide

You can change most of the game's settings and visuals from the file:

### `GameConfig.h`

Here are some examples of what you can modify:

```cpp
// Maze settings
inline const int TILE_SIZE = 64;
inline const int MAZE_WIDTH = 20;
inline const int MAZE_HEIGHT = 15;

// Game limits
inline const int PLAYER_LIVES = 3;
inline const int MAX_ITEMS = 5;
inline const int MAX_ENEMIES = 3;

// Colors
inline const SDL_Color COLOR_PLAYER_FILL = { 0, 200, 0, 255 };
inline const SDL_Color COLOR_ENEMY_FILL = { 255, 0, 0, 255 };

// Game speed
inline const Uint64 ENEMY_MOVE_INTERVAL = 500;
inline const Uint64 ITEM_MOVE_INTERVAL = 500;
```
You can also change the shape of entities (circle, triangle, diamond), the number of lives, score per item, speed of enemies/items, and more.

Maze Layout
In main.cpp, you can edit the maze layout using a 2D vector:

```cpp
std::vector<std::vector<int>> layout = {
    {0,0,0,0,0},
    {0,3,1,4,0},
    {0,1,0,1,0},
    {0,2,1,5,0},
    {0,0,0,0,0}
};
Each number represents a tile type:

0 = Wall
1 = Path
2 = Item
3 = Player start
4 = Goal tile
5 = Enemy
```
You can design your own maze here by changing the values.

## Suggested Experiments
Try modifying the game to:

- Change the player’s color or shape
- Add new enemies or items
- Change the maze layout
- Increase or decrease speed or time
- Make enemies follow the player
This is a great way to practice working with C++ classes, logic, and structure.

## Notes
- All SDL3 and SDL_ttf files are already included in the project folder.
- Do not delete the font.ttf or DLL files.
- If you want a different font style, you can search fonts through google, download any .ttf file and rename them to font.ttf and replace this file
- The project is set up to work out of the box on Windows using Visual Studio 2022.
