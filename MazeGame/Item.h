// Item.h
#pragma once
#include "Entity.h"
#include <vector>

namespace Item {
    void add(int x, int y);
    void clearAll();
    void fillRandom();

    void updateAll();
    void renderAll();

    void checkCollection(int& score);
}