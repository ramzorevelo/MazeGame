// GameConfig.h
// TIP: If you change the layout size in main.cpp (add/remove rows or columns),
// make sure to update MAZE_WIDTH and MAZE_HEIGHT to match,
// or the game may crash or behave unexpectedly.
#pragma once
#include <SDL3/SDL.h>

namespace GameConfig {
	// --- Maze and Tile Settings ---

	// Size of each tile in pixels. This affects how big each cell appears on screen.
	// You can make the game look bigger or smaller by changing this value.
	inline const int TILE_SIZE = 64;

	// Width of the maze in number of tiles (columns).
	// IMPORTANT: This must match the number of columns in the layout vector in main.cpp.
	inline const int MAZE_WIDTH = 20;

	// Height of the maze in number of tiles (rows).
	// IMPORTANT: This must match the number of rows in the layout vector in main.cpp.
	inline const int MAZE_HEIGHT = 15;


	// --- Game Limits ---

// Maximum number of items (yellow circles) placed on the maze.
// These give points when collected.
	inline const int MAX_ITEMS = 5;

	// Maximum number of enemies (red triangles) that move around the maze.
	inline const int MAX_ENEMIES = 3;

	// Number of lives the player starts with.
	inline const int PLAYER_LIVES = 3;


	// --- Item and Enemy Effects ---

	// How many points the player gets for collecting one item.
	inline const int ITEM_SCORE = 10;

	// How many lives the player loses when colliding with an enemy.
	// You can set this to 2 or more if you want enemies to be more dangerous.
	inline const int LIFE_LOSS_ON_HIT = 1;


	// --- Timers (in milliseconds) ---

	// How often items move (if at all). Lower = faster movement.
	inline const Uint64 ITEM_MOVE_INTERVAL = 500;

	// How often enemies move. Lower = faster enemies.
	inline const Uint64 ENEMY_MOVE_INTERVAL = 500;

	// How often the time counter decreases. 1000 = every second.
	inline const Uint64 TIME_DECREASE_INTERVAL = 1000;

	// How long the player becomes invulnerable after getting hit (in ms).
	// During this time, the player can't lose more lives.
	inline const Uint64 INVULNERABLE_DURATION = 1000;


	// --- Displays ---

	// Number of decimal places to show for time played in the end-of-game popup.
	inline const int TIME_PLAYED_DECIMALS = 2;


	// --- Colors --- (Red, Green, Blue, Alpha)

	// Background color of the whole window.
	inline const SDL_Color COLOR_BG = { 0, 0, 0, 255 };

	// Wall color (gray) and walkable path color (white).
	inline const SDL_Color COLOR_WALL = { 100, 100, 100, 255 };
	inline const SDL_Color COLOR_PATH = { 255, 255, 255, 255 };

	// Player appearance colors (fill and outline).
	inline const SDL_Color COLOR_PLAYER_FILL = { 0, 200, 0, 255 };
	inline const SDL_Color COLOR_PLAYER_OUTLINE = { 0, 100, 0, 255 };

	// Enemy color (fill only, outline is hardcoded black).
	inline const SDL_Color COLOR_ENEMY_FILL = { 255, 0, 0, 255 };

	// Item and goal colors (fill only).
	inline const SDL_Color COLOR_ITEM_FILL = { 255, 255, 0, 255 };
	inline const SDL_Color COLOR_GOAL_FILL = { 0, 0, 255, 255 };

	// Text color used for UI labels (score, time, lives).
	inline const SDL_Color COLOR_TEXT = { 255, 255, 255, 255 };


	// --- Font Settings ---

	// Path to the font file used for UI text.
	// You usually don't need to change this unless you have a different font file 
	// in a different directory
	inline const char* FONT_PATH = "font.ttf";

	// Size of the font (in points). Increase for larger UI text.
	inline const int FONT_SIZE = 24;

	
	// --- UI ---

	// If the maze height is greater than this value,
	// the UI will automatically switch to a compact layout (1 line for all labels).
	// Otherwise, it will use a multi-line layout (e.g. 3 lines: Score, Time, Lives).
	inline const int UI_COMPACT_THRESHOLD = 13;

	// Returns how many lines the UI should have based on maze height.
	// You usually don't need to change this function.
	inline int getUiLines() {
		return (MAZE_HEIGHT > UI_COMPACT_THRESHOLD) ? 1 : 3;
	}

	// Height (in pixels) of each UI line (used for spacing).
	// Increase this if your text looks too cramped or decrease it for tighter layout.
	inline const int UI_LINE_HEIGHT = 30;

	// Total vertical space reserved for the UI (above the maze).
	// This is automatically calculated based on how many lines are shown.
	inline const int UI_OFFSET_Y = UI_LINE_HEIGHT * getUiLines();

	// The width of the game window, based on how many tiles wide the maze is.
	// You usually don't need to change this directly. Change MAZE_WIDTH or TILE_SIZE instead.
	inline const int WINDOW_WIDTH = MAZE_WIDTH * TILE_SIZE;

	// The height of the game window, based on maze height and UI space.
	// You usually don't need to change this directly. Change MAZE_HEIGHT or TILE_SIZE instead.
	inline const int WINDOW_HEIGHT = MAZE_HEIGHT * TILE_SIZE + UI_OFFSET_Y;
}
