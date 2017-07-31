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
	borderWidth = 0.0f;
	paddlePos = glm::vec3(0, r->screenH *0.8f ,0);
	paddleScale = glm::vec3(10, 3, 10);
	ballPos = glm::vec3(0, r->screenH * 0.5,0);
	ballVel = glm::normalize(glm::vec3(0.5, -1, 0));
	renderer = r;
	brickPrimHdls = new int[numBricks];
	memset(brickPrimHdls, -1, sizeof(int) * numBricks);


	paddlePrimHdl = PrimitiveManager::Get()->NewPrimitive(MeshManager::Get()->GetRectMesh(r));
	ballPrimHdl = PrimitiveManager::Get()->NewPrimitive(MeshManager::Get()->GetCircleMesh(r));

	SetPrimPos(ballPrimHdl, ballPos);
	SetPrimScale(ballPrimHdl, glm::vec3(3, 3, 3));

	SetPrimPos(paddlePrimHdl, paddlePos);
	SetPrimScale(paddlePrimHdl, paddleScale);

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
	for (int i = 0; i < numBricks; ++i)
	{
		if (brickPrimHdls[i] > -1)
		{
			PrimitiveManager::Get()->DestroyPrimitive(brickPrimHdls[i]);
		}
	}
	PrimitiveManager::Get()->DestroyPrimitive(paddlePrimHdl);
	PrimitiveManager::Get()->DestroyPrimitive(ballPrimHdl);

	delete[] brickPrimHdls;

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
		paddlePos -= glm::vec3(0.5, 0, 0);
		paddlePos.x = paddlePos.x < -renderer->screenW+paddleScale.x ? -renderer->screenW+paddleScale.x : paddlePos.x;
		SetPrimPos(paddlePrimHdl, paddlePos);
	}

	if (GetKey(KeyCode::KEY_RIGHT))
	{
		paddlePos += glm::vec3(0.5, 0, 0);
		paddlePos.x = (paddlePos.x) > renderer->screenW-paddleScale.x ? renderer->screenW-paddleScale.x : paddlePos.x;
		SetPrimPos(paddlePrimHdl, paddlePos);
	}

	glm::vec3 frameVel = ballVel * deltaTime * 1000.0f;
	ballPos += frameVel;

	if (ballPos.y <= -renderer->screenH + 1)
	{
		ballPos -= frameVel;
		ballVel.y *= -1;
	}
	
	if (ballPos.y >= renderer->screenH - 1)
	{
		ballPos -= frameVel;
		ballVel.y *= -1;
		gameOver = true;

	}
	
	if (ballPos.x >= renderer->screenW - borderWidth -1)
	{
		ballPos -= frameVel;
		ballVel.x *= -1;
	}

	if (ballPos.x <= -renderer->screenW + 1 + borderWidth)
	{
		ballPos -= frameVel;
		ballVel.x *= -1;
	}

	for (int i = 0; i < numBricks; ++i)
	{
		if (brickPrimHdls[i] > -1 && BallIntersectsRect(brickPrimHdls[i], ballPos))
		{
			glm::vec3 brickPos = PrimitiveManager::Get()->primitives[brickPrimHdls[i]].pos;
			glm::vec3 brickScale = PrimitiveManager::Get()->primitives[brickPrimHdls[i]].scale;
			
			ballPos -= ballVel * 0.5f;
			while (BallIntersectsRect(brickPrimHdls[i], ballPos))
			{
				ballPos -= ballVel * 0.5f;
			}
			SetPrimPos(ballPrimHdl, ballPos);
			
			ballVel.y *= -1;
			
			PrimitiveManager::Get()->DestroyPrimitive(brickPrimHdls[i]);
			brickPrimHdls[i] = -1;
		}
	}

	if (BallIntersectsRect(paddlePrimHdl, ballPos))
	{
		ballPos -= frameVel;
		SetPrimPos(ballPrimHdl, ballPos);
		ballVel.y *= -1;
	}

	SetPrimPos(ballPrimHdl, ballPos);

	
}

void BreakoutGame::draw() const
{
	PrimitiveManager::Get()->SubmitPrimitives(renderer);
}

bool BreakoutGame::isGameOver() const
{
	if (gameOver) return true;

	for (int i = 0; i < numBricks; ++i)
	{
		if (brickPrimHdls[i] > -1) return false;
	}

	return true;
}

void BreakoutGame::restart()
{

}