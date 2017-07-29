#include "BreakoutGame.h"
#include "os_support.h"
#include "Renderer.h"
#include "stdafx.h"
#include "Primitive.h"
#include "MeshManager.h"
#include "os_support.h"

BreakoutGame::BreakoutGame(Renderer* r)
{
	numBricks = 50;

	paddlePos = glm::vec3(0, 85,0);
	ballPos = glm::vec3(0, 49,0);
	ballVel = glm::normalize(glm::vec3(0.5, -1, 0));
	renderer = r;
	brickPrimHdls = new int[numBricks];
	memset(brickPrimHdls, -1, sizeof(int) * numBricks);

	paddlePrimHdl = PrimitiveManager::Get()->NewPrimitive(MeshManager::Get()->GetRectMesh(r));
	ballPrimHdl = PrimitiveManager::Get()->NewPrimitive(MeshManager::Get()->GetCircleMesh(r));

	SetPrimPos(ballPrimHdl, ballPos);
	SetPrimScale(ballPrimHdl, glm::vec3(3, 3, 3));

	SetPrimPos(paddlePrimHdl, paddlePos);
	SetPrimScale(paddlePrimHdl, glm::vec3(10, 3, 10));

	//construct brick field
	for (int i = 0; i < numBricks; ++i)
	{
		int b = PrimitiveManager::Get()->NewPrimitive(MeshManager::Get()->GetRectMesh(r));
		glm::vec3 p = glm::vec3(-110 + (i %10) * 25, -50 + (i/10) * 10,0);

		SetPrimPos(b, p);
		SetPrimCol(b, glm::vec4((i / 10) / 5.0f, (i % 10) / 10.0f, 1.0f, 1.0f));
		SetPrimScale(b, glm::vec3(10, 3, 10));
		brickPrimHdls[i] = b;
	}
}

BreakoutGame::~BreakoutGame()
{

}

float clamp(float a, float min, float max)
{
	return a < min ? min : (a > max ? max : a);
}

bool BallIntersectsRect(int rectPrim, glm::vec3& ballPos)
{
	glm::vec3 rectPos = PrimitiveManager::Get()->primitives[rectPrim].pos;
	glm::vec3 rectScale = PrimitiveManager::Get()->primitives[rectPrim].scale;

	float left = rectPos.x - rectScale.x;
	float right = rectPos.x + rectScale.x;
	float top = rectPos.y - rectScale.y;
	float bottom = rectPos.y + rectScale.y;

	// Find the closest point to the circle within the rectangle
	float closestX = clamp(ballPos.x, left, right);
	float closestY = clamp(ballPos.y, top, bottom);

	// Calculate the distance between the circle's center and this closest point
	float distanceX = ballPos.x - closestX;
	float distanceY = ballPos.y - closestY;

	// If the distance is less than the circle's radius, an intersection occurs
	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
	return distanceSquared < (2 * 2);
}


void BreakoutGame::tick(float deltaTime)
{
	if (GetKey(KeyCode::KEY_LEFT))
	{
		paddlePos -= glm::vec3(1, 0, 0);
		paddlePos.x = paddlePos.x < -110 ? -110 : paddlePos.x;
		SetPrimPos(paddlePrimHdl, paddlePos);
	}

	if (GetKey(KeyCode::KEY_RIGHT))
	{
		paddlePos += glm::vec3(1, 0, 0);
		paddlePos.x = paddlePos.x > 110 ? 110 : paddlePos.x;
		SetPrimPos(paddlePrimHdl, paddlePos);
	}

	ballPos += ballVel * deltaTime * 1000.0f;

	if (ballPos.y <= -100)
	{
		ballPos -= ballVel * deltaTime * 1000.0f;
		ballVel.y *= -1;
	}
	
	if (ballPos.y >= 100)
	{
		ballPos -= ballVel * deltaTime * 1000.0f;
		ballVel.y *= -1;
	}
	
	if (ballPos.x >= 130)
	{
		ballPos -= ballVel * deltaTime * 1000.0f;
		ballVel.x *= -1;
	}

	if (ballPos.x <= -130)
	{
		ballPos -= ballVel * deltaTime * 1000.0f;
		ballVel.x *= -1;
	}

	for (int i = 0; i < numBricks; ++i)
	{
		if (brickPrimHdls[i] > -1 && BallIntersectsRect(brickPrimHdls[i], ballPos))
		{
			PrimitiveManager::Get()->DestroyPrimitive(brickPrimHdls[i]);
			brickPrimHdls[i] = -1;
			ballVel *= -1;
		}
	}

	if (BallIntersectsRect(paddlePrimHdl, ballPos))
	{
		ballPos -= ballVel * deltaTime * 1000.0f;
		ballVel *= -1;
	}

	SetPrimPos(ballPrimHdl, ballPos);

	
}

void BreakoutGame::draw() const
{
	PrimitiveManager::Get()->SubmitPrimitives(renderer);
}

bool BreakoutGame::isGameOver() const
{
	for (int i = 0; i < numBricks; ++i)
	{
		if (brickPrimHdls[i] > -1) return true;
	}
	return false;
}

void BreakoutGame::restart()
{

}