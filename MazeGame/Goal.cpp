// Goal.cpp
#include "Goal.h"
#include "Player.h"
#include "Entity.h"
#include "GameConfig.h"

namespace {
    Entity goal(0, 0, ShapeType::DIAMOND, GameConfig::COLOR_GOAL_FILL, { 0, 0, 0, 255 });
}

void Goal::setPosition(int x, int y) {
    goal.setPosition(x, y);
}

void Goal::render() {
    goal.render();
}

bool Goal::checkReached() {
    return Player::getX() == goal.getX() && Player::getY() == goal.getY();
}