#pragma once
#include "stdafx.h"
//game logic only. not responsible for loading assets, creating meshes, etc. 
//only responsible for consuming them, and passing along data to the renderer


class BreakoutGame
{
public:
	BreakoutGame();
	~BreakoutGame();

	void tick(float deltaTime);
	void draw(class Renderer* renderer) const;

	bool isGameOver() const;
	void restart();

private:
	bool BallIntersectsRect(int rectPrimHdl);


	int numBricks;

	glm::vec3 paddlePos;
	glm::vec3 ballPos;
	glm::vec3 ballVel;
	glm::vec3 paddleScale;
	float borderWidth;
	float ballRad;
	int paddlePrimHdl;
	int ballPrimHdl;
	bool gameOver;
	int* brickPrimHdls;
};