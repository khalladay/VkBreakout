#pragma once

//only responsible to manipulating primitive data

namespace Breakout
{
	void tick(float deltaTime);
	bool isGameOver();
	void newGame();
	void draw();
}