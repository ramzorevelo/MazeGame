// Item.cpp
#include "Item.h"
#include "Maze.h"
#include "Player.h"
#include "GameConfig.h"
#include "VisualEffect.h"

#include <cstdlib>

namespace {
    struct ItemData : Entity {
        int direction; // 0 = right, 1 = down, 2 = left, 3 = up

        ItemData(int x, int y)
            : Entity(x, y, ShapeType::CIRCLE, GameConfig::COLOR_ITEM_FILL, { 0, 0, 0, 255 }),
            direction(rand() % 4) {
        }
    };

    std::vector<ItemData> items;
    Uint64 lastMoveTime = 0;
}

void Item::add(int x, int y) {
    if (items.size() < GameConfig::MAX_ITEMS) {
        items.emplace_back(x, y);
    }
}

void Item::clearAll() {
    items.clear();
}

void Item::fillRandom() {
    while (items.size() < GameConfig::MAX_ITEMS) {
        int x = rand() % GameConfig::MAZE_WIDTH;
        int y = rand() % GameConfig::MAZE_HEIGHT;
        if (Maze::isWalkable(x, y)) {
            bool occupied = false;
            for (auto& i : items) {
                if (i.getX() == x && i.getY() == y) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) {
                add(x, y);
            }
        }
    }
}

void Item::updateAll() {
    Uint64 now = SDL_GetTicks();
    if (now - lastMoveTime < GameConfig::ITEM_MOVE_INTERVAL) return;

    for (auto& item : items) {
        int newX = item.getX();
        int newY = item.getY();

        switch (item.direction) {
        case 0: newX++; break;
        case 1: newY++; break;
        case 2: newX--; break;
        case 3: newY--; break;
        }

        if (!Maze::isWalkable(newX, newY)) {
            item.direction = rand() % 4;
        }
        else {
            item.setPosition(newX, newY);
        }
    }

    lastMoveTime = now;
}

void Item::renderAll() {
    for (const auto& item : items) {
        item.render();
    }
}

void Item::checkCollection(int& score) {
    int px = Player::getX();
    int py = Player::getY();

    for (size_t i = 0; i < items.size(); ) {
        if (items[i].getX() == px && items[i].getY() == py) {
            score += GameConfig::ITEM_SCORE;

            // Add visual effect
            VisualEffect::EffectConfig config;
            config.color = { 0, 255, 0, 255 }; // Green text for points
            config.fontSize = 28;
            config.riseSpeed = 0.1f;
            config.duration = 1500;

            VisualEffect::add("+" + std::to_string(GameConfig::ITEM_SCORE),
                items[i].getX(), items[i].getY(), config);

            items.erase(items.begin() + i); // remove collected item
        }
        else {
            ++i;
        }
    }
}
