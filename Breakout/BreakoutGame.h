#pragma once
#include "stdafx.h"
#include <vector>
//game logic only. not responsible for loading assets, creating meshes, etc. 
//only responsible for consuming them, and passing along data to the renderer

class BreakoutGame
{
public:
	BreakoutGame(class Renderer* renderer);
	~BreakoutGame();

	void tick(float deltaTime);
	void draw() const;

	bool isGameOver() const;
	void restart();

	Renderer* renderer;

private:
	glm::vec3 paddlePos;
	glm::vec2 paddleVel;
	glm::vec2 ballPos;
	glm::vec2 ballVel;
	float ballRad;

	int paddlePrimHdl;
	int ballPrimHdl;

	std::vector<int> brickPrimHdls;
	std::vector<glm::vec2> brickPos;
};