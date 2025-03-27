#include "game.h"

// Define global variables
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr; // Font for text rendering
SDL_Color textColor = { 255, 255, 255, 255 }; // White text color

int maze[MAZE_HEIGHT][MAZE_WIDTH];
int playerX, playerY;
int goalX, goalY;
int score = 0;
int health = 100;
int playerLives = 3; // Define player lives globally
bool gameOver = false;
bool gameWon = false;
std::vector<Item> items;
std::vector<Enemy> enemies; // Define enemies globally
bool isInvulnerable = false;
Uint64 invulnerableStartTime = 0;
std::vector<VisualEffect> visualEffects;
bool initSDL() {
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (!TTF_Init()) {
        SDL_Log("SDL_ttf could not initialize! TTF_Error: %s", SDL_GetError());
        return false;
    }

    // Load font (ensure font.ttf is in the project directory)
    font = TTF_OpenFont("font.ttf", 24); // 24 is the font size
    if (!font) {
        SDL_Log("Failed to load font! TTF_Error: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Maze Game", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        return false;
    }

    return true;
}

void closeSDL() {
    // Clean up SDL_ttf
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();

    // Clean up SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
SDL_FColor toFColor(SDL_Color c) {
    return SDL_FColor{
        c.r / 255.0f,
        c.g / 255.0f,
        c.b / 255.0f,
        c.a / 255.0f
    };
}
void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderPoint(renderer, (float)(centerX + dx), (float)(centerY + dy));
            }
        }
    }
}
void drawFilledTriangle(SDL_Renderer* renderer, SDL_FPoint p1, SDL_FPoint p2, SDL_FPoint p3, SDL_Color color) {
    SDL_Vertex verts[3];

    verts[0].position = p1;
    verts[1].position = p2;
    verts[2].position = p3;

    for (int i = 0; i < 3; ++i) {
        verts[i].color = toFColor(color);
        verts[i].tex_coord = { 0, 0 }; // Not using textures
    }

    SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
}
void drawEnemyTriangle(int x, int y, int direction, SDL_Color color, int yOffset) {
    int centerX = x * TILE_SIZE + TILE_SIZE / 2;
    int centerY = y * TILE_SIZE + yOffset + TILE_SIZE / 2;
    int half = TILE_SIZE / 3;

    SDL_FPoint points[4]; // 3 points + closing point

    switch (direction) {
    case PATH_UP:
        points[0] = { (float)centerX, (float)(centerY - half) };
        points[1] = { (float)(centerX - half), (float)(centerY + half) };
        points[2] = { (float)(centerX + half), (float)(centerY + half) };
        break;
    case PATH_DOWN:
        points[0] = { (float)centerX, (float)(centerY + half) };
        points[1] = { (float)(centerX - half), (float)(centerY - half) };
        points[2] = { (float)(centerX + half), (float)(centerY - half) };
        break;
    case PATH_LEFT:
        points[0] = { (float)(centerX - half), (float)centerY };
        points[1] = { (float)(centerX + half), (float)(centerY - half) };
        points[2] = { (float)(centerX + half), (float)(centerY + half) };
        break;
    case PATH_RIGHT:
        points[0] = { (float)(centerX + half), (float)centerY };
        points[1] = { (float)(centerX - half), (float)(centerY - half) };
        points[2] = { (float)(centerX - half), (float)(centerY + half) };
        break;
    }


    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderLines(renderer, points, 4);
    SDL_RenderLines(renderer, points, 4); // Fill not available, but outline works
}

void drawFilledDiamond(SDL_Renderer* renderer, int centerX, int centerY, int size, SDL_Color color) {
    SDL_FPoint points[4] = {
        { (float)centerX,         (float)(centerY - size) }, // Top
        { (float)(centerX + size), (float)centerY },          // Right
        { (float)centerX,         (float)(centerY + size) }, // Bottom
        { (float)(centerX - size), (float)centerY }           // Left
    };

    SDL_Vertex verts[6]; // 2 triangles to form a diamond

    verts[0].position = points[0]; verts[0].color = toFColor(color);
    verts[1].position = points[1]; verts[1].color = toFColor(color);
    verts[2].position = points[2]; verts[2].color = toFColor(color);

    verts[3].position = points[2]; verts[3].color = toFColor(color);
    verts[4].position = points[3]; verts[4].color = toFColor(color);
    verts[5].position = points[0]; verts[5].color = toFColor(color);

    for (int i = 0; i < 6; ++i)
        verts[i].tex_coord = { 0, 0 };

    SDL_RenderGeometry(renderer, nullptr, verts, 6, nullptr, 0);
}
void renderText(const std::string& text, int x, int y) {
    // Create a surface from the text
    SDL_Color fg = { 0, 0, 0, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), text.size(), textColor);
    if (!textSurface) {
        SDL_Log("Unable to render text surface! TTF_Error: %s", SDL_GetError());
        return;
    }

    // Create a texture from the surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_Log("Unable to create texture from rendered text! SDL_Error: %s", SDL_GetError());
        SDL_DestroySurface(textSurface);
        return;
    }

    // Get the width and height of the text
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    // Free the surface (we don't need it anymore)
    SDL_DestroySurface(textSurface);

    // Define the destination rectangle for rendering
    SDL_FRect dstRect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(textWidth), static_cast<float>(textHeight) };

    // Render the text texture
    SDL_RenderTexture(renderer, textTexture, nullptr, &dstRect);

    // Free the texture
    SDL_DestroyTexture(textTexture);
}

void renderMaze() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render score text *FIRST*
    std::string scoreText = "Score: " + std::to_string(score);
    renderText(scoreText, 10, 10); // Display at top (y = 10)

    // Render time remaining text (time is based on health)
    std::string timeText = "Time Remaining: " + std::to_string(health) + "s";
    renderText(timeText, 10, 40); // Display below score (y = 40)

    // Render lives text
    std::string livesText = "Lives: " + std::to_string(playerLives);
    renderText(livesText, 10, 70); // Display below time (y = 70)

    // Render maze *AFTER* the text, with a vertical offset.  This is crucial.
    int yOffset = 100; // Offset the maze rendering by 100 pixels downwards
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            SDL_FRect tile = { static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE + yOffset), static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE) };
            if (maze[y][x] == WALL) {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray for walls
            }
            else if (maze[y][x] == PATH) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for paths
            }
            SDL_RenderFillRect(renderer, &tile);
        }
    }

    // Render items based on the items vector
    for (const Item& item : items) {
        int centerX = item.x * TILE_SIZE + TILE_SIZE / 2;
        int centerY = item.y * TILE_SIZE + yOffset + TILE_SIZE / 2;
        int radius = TILE_SIZE / 3;

        SDL_Color itemColor = { 255, 255, 0, 255 }; // Yellow
        drawFilledCircle(renderer, centerX, centerY, radius, itemColor);
    }
    // Render enemies based on the enemies vector
    for (const Enemy& enemy : enemies) {
        // Inside enemy rendering loop
        int centerX = enemy.x * TILE_SIZE + TILE_SIZE / 2;
        int centerY = enemy.y * TILE_SIZE + yOffset + TILE_SIZE / 2;
        int half = TILE_SIZE / 3;

        SDL_FPoint p1, p2, p3;
        switch (enemy.direction) {
        case PATH_UP:
            p1 = { (float)centerX, (float)(centerY - half) };
            p2 = { (float)(centerX - half), (float)(centerY + half) };
            p3 = { (float)(centerX + half), (float)(centerY + half) };
            break;
        case PATH_DOWN:
            p1 = { (float)centerX, (float)(centerY + half) };
            p2 = { (float)(centerX - half), (float)(centerY - half) };
            p3 = { (float)(centerX + half), (float)(centerY - half) };
            break;
        case PATH_LEFT:
            p1 = { (float)(centerX - half), (float)centerY };
            p2 = { (float)(centerX + half), (float)(centerY - half) };
            p3 = { (float)(centerX + half), (float)(centerY + half) };
            break;
        case PATH_RIGHT:
            p1 = { (float)(centerX + half), (float)centerY };
            p2 = { (float)(centerX - half), (float)(centerY - half) };
            p3 = { (float)(centerX - half), (float)(centerY + half) };
            break;
        }

        SDL_Color enemyColor = { 255, 0, 0, 255 };
        drawFilledTriangle(renderer, p1, p2, p3, enemyColor);
    }

    // Render player (with blinking effect if invulnerable)
    if (!isInvulnerable || (SDL_GetTicks() / 100) % 2 == 0) {
        // Draw player circle here 
        int centerX = playerX * TILE_SIZE + TILE_SIZE / 2;
        int centerY = playerY * TILE_SIZE + yOffset + TILE_SIZE / 2;
        int radius = TILE_SIZE / 3;

        SDL_Color fill = { 0, 200, 0, 255 }; // Green
        SDL_Color outline = { 0, 100, 0, 255 }; // Darker green

        // Draw filled circle
        drawFilledCircle(renderer, centerX, centerY, radius, fill);

        // Optional: draw simple outline manually
        for (int r = radius + 1; r <= radius + 2; r++) {
            drawFilledCircle(renderer, centerX, centerY, r, outline);
        }
    }

    // Render goal (with offset)
    int centerX = goalX * TILE_SIZE + TILE_SIZE / 2;
    int centerY = goalY * TILE_SIZE + yOffset + TILE_SIZE / 2;
    int size = TILE_SIZE / 3;

    SDL_Color goalColor = { 0, 0, 255, 255 }; // Blue
    drawFilledDiamond(renderer, centerX, centerY, size, goalColor);

    // Render visual effects
    Uint64 currentTime = SDL_GetTicks();
    for (size_t i = 0; i < visualEffects.size(); ) {
        VisualEffect& effect = visualEffects[i];
        Uint64 elapsed = currentTime - effect.startTime;

        if (elapsed > effect.duration) {
            // Remove expired effect
            visualEffects.erase(visualEffects.begin() + i);
            continue;
        }

        // Update position and alpha
        effect.y -= effect.riseSpeed;
        effect.alpha = 255 * (1.0f - (float)elapsed / effect.duration);

        // Render text with transparency
        SDL_Color color = { 255, 255, 0, static_cast<Uint8>(effect.alpha) };
        SDL_Surface* surface = TTF_RenderText_Solid(font, effect.text.c_str(), effect.text.length(), color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_FRect dstRect = { effect.x, effect.y, static_cast<float>(surface->w), static_cast<float>(surface->h) };
                SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
                SDL_DestroyTexture(texture);
            }
            SDL_DestroySurface(surface);
        }

        ++i;
    }
    // Update screen
    SDL_RenderPresent(renderer);
}

void handleInput(SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        int newX = playerX;
        int newY = playerY;

        switch (event.key.key) {
        case SDLK_UP: newY--; break;
        case SDLK_DOWN: newY++; break;
        case SDLK_LEFT: newX--; break;
        case SDLK_RIGHT: newX++; break;
        }

        movePlayer(newX, newY);
    }
}
