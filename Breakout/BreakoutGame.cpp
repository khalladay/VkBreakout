#include "BreakoutGame.h"
#include "os_support.h"
#include "Renderer.h"
#include "stdafx.h"
#include "Primitive.h"
#include "MeshManager.h"
#include "os_support.h"

float clamp(float a, float min, float max)
{
	return a < min ? min : (a > max ? max : a);
}

BreakoutGame::BreakoutGame(Renderer* r)
	: renderer(r)
{
	//setup game config
	{
		numBricks = 50;
		borderWidth = 0.0f;

		paddlePos = glm::vec3(0, r->screenH *0.8f, 0);
		paddleScale = glm::vec3(10, 1, 10);

		ballPos = glm::vec3(0, r->screenH * 0.5, 0);
		ballVel = glm::normalize(glm::vec3(0.5, -1, 0));
		ballRad = 1.5f;
	}

	//paddle/ball initialization
	{
		paddlePrimHdl = PM->NewPrimitive(MeshManager::Get()->GetRectMesh(r));
		ballPrimHdl = PM->NewPrimitive(MeshManager::Get()->GetCircleMesh(r));

		PM->SetPrimPos(ballPrimHdl, ballPos);
		PM->SetPrimScale(ballPrimHdl, glm::vec3(ballRad * 2, ballRad * 2, ballRad * 2));

		PM->SetPrimPos(paddlePrimHdl, paddlePos);
		PM->SetPrimScale(paddlePrimHdl, paddleScale);
	}

	//construct brick field
	{
		brickPrimHdls = new int[numBricks];
		memset(brickPrimHdls, -1, sizeof(int) * numBricks);

		for (int i = 0; i < numBricks; ++i)
		{
			int b = PM->NewPrimitive(MeshManager::Get()->GetRectMesh(r));
			glm::vec3 p = glm::vec3(-110 + (i % 10) * 25, -50 + (i / 10) * 10, 0);

			PM->SetPrimPos(b, p);
			PM->SetPrimCol(b, glm::vec4((i / 10) / 5.0f, (i % 10) / 10.0f, 1.0f, 1.0f));
			PM->SetPrimScale(b, glm::vec3(10, 3, 10));
			brickPrimHdls[i] = b;
		}
	}
}

BreakoutGame::~BreakoutGame()
{
	for (int i = 0; i < numBricks; ++i)
	{
		if (brickPrimHdls[i] > -1)
		{
			PM->DestroyPrimitive(brickPrimHdls[i]);
		}
	}
	PM->DestroyPrimitive(paddlePrimHdl);
	PM->DestroyPrimitive(ballPrimHdl);

	delete[] brickPrimHdls;

}

bool BreakoutGame::BallIntersectsRect(int rectPrimHdl)
{
	glm::vec3 rectPos = PM->GetPrimPos(rectPrimHdl); 
	glm::vec3 rectScale = PM->GetPrimScale(rectPrimHdl);

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
	return distanceSquared < (ballRad * ballRad);
}


void BreakoutGame::tick(float deltaTime)
{
	//ball movement and intersection
	glm::vec3 frameVel = ballVel * deltaTime * 10.0f;

	ballPos += frameVel;

	//clamp the ball to the screen / it bounces off edges
	if (ballPos.y <= -renderer->screenH + ballRad)
	{
		ballPos -= frameVel;
		ballVel.y *= -1;
	}
	
	//treat bottom edge separately, it's game over if ball hits it
	if (ballPos.y >= renderer->screenH - ballRad)
	{
		ballPos -= frameVel;
		ballVel.y *= -1;
		gameOver = true;
	}
	
	if (ballPos.x >= renderer->screenW - ballRad
		|| ballPos.x <= -renderer->screenW + ballRad)
	{
		ballPos -= frameVel;
		ballVel.x *= -1;
	}

	//check for collision with bricks, kill brick if hit
	for (int i = 0; i < numBricks; ++i)
	{
		if (brickPrimHdls[i] > -1 && BallIntersectsRect(brickPrimHdls[i]))
		{
			glm::vec3 brickPos = PM->GetPrimPos(brickPrimHdls[i]);  
			glm::vec3 brickScale = PM->GetPrimScale(brickPrimHdls[i]);
			
			ballPos -= ballVel * 0.5f;
			PM->SetPrimPos(ballPrimHdl, ballPos);
			
			ballVel.y *= -1;
			
			PM->DestroyPrimitive(brickPrimHdls[i]);
			brickPrimHdls[i] = -1;
		}
	}

	if (BallIntersectsRect(paddlePrimHdl))
	{
		//paddle doesn't get killed on hit, so we need to back up
		//until we *just* aren't intersecting before bouncing
		while (BallIntersectsRect(paddlePrimHdl))
		{
			ballPos -= ballVel * 0.1f;
		}

		float xIntersect = clamp(ballPos.x - paddlePos.x, -0.5, 0.5);
	
		if (xIntersect * xIntersect > (0.3 * 0.3))
		{
			ballVel.x = xIntersect;
			ballVel = glm::normalize(ballVel);
		}

		ballVel.y *= -1;
	}

	PM->SetPrimPos(ballPrimHdl, ballPos);


	//move paddle
	if (GetKey(KeyCode::KEY_LEFT))
	{
		paddlePos -= glm::vec3(0.5f * deltaTime * 30.0f, 0, 0);
	}
	else if (GetKey(KeyCode::KEY_RIGHT))
	{
		paddlePos += glm::vec3(0.5f * deltaTime * 30.0f, 0, 0);
	}

	paddlePos.x = clamp(paddlePos.x, -renderer->screenW + paddleScale.x, renderer->screenW - paddleScale.x);
	PM->SetPrimPos(paddlePrimHdl, paddlePos);
}

void BreakoutGame::draw() const
{
	PM->SubmitPrimitives(renderer);
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