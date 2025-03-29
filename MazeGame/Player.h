// Player.h
#pragma once
#include "Entity.h"

namespace Player {
	void setPosition(int x, int y);
	void move(int dx, int dy);
	void render(bool invulnerable);
	void update();

	int getX();
	int getY();
}