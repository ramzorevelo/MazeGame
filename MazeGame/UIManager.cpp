// UIManager.cpp
#include "UIManager.h"
#include "Game.h"
#include "GameConfig.h"
#include "Renderer.h"

#include <vector>
#include <functional>

// Struct that represents a UI label with dynamic value
struct UILabel {
    std::string prefix;
    std::function<std::string()> getValue;
    int x, y;
};

// Internal list of labels
namespace {
    std::vector<UILabel> labels;
}

void UIManager::setupDefaultLabels() {
    labels.clear();

    bool useCompactLayout = GameConfig::MAZE_HEIGHT > GameConfig::UI_COMPACT_THRESHOLD;

    if (useCompactLayout) {
        // All in one line, spaced horizontally
        labels.push_back({ "Score: ", [] { return "0"; }, 10, 0 });
        labels.push_back({ "Time: ", [] { return "0s"; }, 200, 0 });
        labels.push_back({ "Lives: ", [] { return "0"; }, 400, 0 });

        // Adjust UI config dynamically if needed (optional)
        // You can even reduce UI_OFFSET_Y here if you want
    }
    else {
        // Multi-line layout
        labels.push_back({ "Score: ", [] { return "0"; }, 10, 0 });
        labels.push_back({ "Time Remaining: ", [] { return "0s"; }, 10, 30 });
        labels.push_back({ "Lives: ", [] { return "0"; }, 10, 60 });
    }
}

void UIManager::renderAll(int score, int timeRemaining, int lives) {
    TTF_Font* font = Game::getFont();
    SDL_Renderer* renderer = Game::getRenderer();

    // Update dynamic values
    labels[0].getValue = [score] { return std::to_string(score); };
    labels[1].getValue = [timeRemaining] { return std::to_string(timeRemaining) + "s"; };
    labels[2].getValue = [lives] { return std::to_string(lives); };

    for (const UILabel& label : labels) {
        std::string text = label.prefix + label.getValue();
        Renderer::renderText(text, label.x, label.y, GameConfig::COLOR_TEXT);
    }
}
