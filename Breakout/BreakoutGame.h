#pragma once

//only responsible to manipulating primitive data
class Renderer;

namespace Breakout
{
	void tick(float deltaTime);
	bool isGameOver();
	void newGame();
	void draw(Renderer* renderer);
}