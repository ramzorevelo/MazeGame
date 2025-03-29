// UIManager.h
#pragma once
#include <string>

namespace UIManager {
    // Setup default text labels (Score, Time Remaining, Lives)
    void setupDefaultLabels();

    // Render all UI text
    void renderAll(int score, int timeRemaining, int lives);
}
