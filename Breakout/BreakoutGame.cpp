#include "BreakoutGame.h"
#include "stdafx.h"

#include "Renderer.h"
#include "Primitive.h"
#include "Mesh.h"
#include "os_support.h"

#include "vkh.h"

using namespace Primitive;
using namespace OS;

namespace Breakout
{
	struct GameData
	{
		int* brickPrimHdls;
		glm::vec3 paddlePos;
		glm::vec3 ballPos;
		glm::vec3 ballVel;
		glm::vec3 paddleScale;
		float ballRad;
		int paddlePrimHdl;
		int ballPrimHdl;
		int numBricks;
		bool gameOver;
	};

	static GameData state;

	inline float clamp(float a, float min, float max)
	{
		return a < min ? min : (a > max ? max : a);
	}

	void newGame()
	{
		//cleanup from previous game if it exists
		{
			if (state.brickPrimHdls)
			{
				delete[] state.brickPrimHdls;
			}

			destroyAllPrimitives();
			state.gameOver = false;
		}

		//setup game config
		{
			state.numBricks = MAX_PRIMS;

			state.paddlePos = glm::vec3(0, 100 * 0.8f, 0);
			state.paddleScale = glm::vec3(10, 1, 10);

			state.ballPos = glm::vec3(0, 100 * 0.65, 0);
			state.ballVel = glm::normalize(glm::vec3(0.5, -1, 0)) * 0.1f;
			state.ballRad = 1.5f;
		}

		//paddle/ball initialization
		{
			state.paddlePrimHdl = newPrimitive(GetRectMesh());
			state.ballPrimHdl = newPrimitive(GetCircleMesh());

			setPrimPos(state.ballPrimHdl, state.ballPos);
			int scale = (int)(state.ballRad * 2);
			setPrimScale(state.ballPrimHdl, glm::vec3(scale, scale, scale));

			setPrimPos(state.paddlePrimHdl, state.paddlePos);
			setPrimScale(state.paddlePrimHdl, state.paddleScale);
		}

		//construct brick field
		{
			state.brickPrimHdls = new int[state.numBricks];
			memset(state.brickPrimHdls, -1, sizeof(int) * state.numBricks);

#if STRESS_TEST
			for (int i = 0; i < state.numBricks; ++i)
			{
				int b = newPrimitive(GetRectMesh());
				glm::vec3 p = glm::vec3(-160 + (i % 110) * 3.0f, -95 + (i / 110) * 3.0, 0);

				setPrimPos(b, p);
				setPrimCol(b, glm::vec4((i / 10) * 5.0f, (i % 10) / 10.0f, 1.0f, 1.0f));
				setPrimScale(b, glm::vec3(1.25, 0.75f, 1.0f));
				state.brickPrimHdls[i] = b;
			}
#else
			for (int i = 0; i < state.numBricks; ++i)
			{
				int b = newPrimitive(GetRectMesh());
				glm::vec3 p = glm::vec3(-150 + (i % 20) * 15, -85 + (i / 20) * 5, 0);

				setPrimPos(b, p);
				setPrimCol(b, glm::vec4((i / 10) / 5.0f, (i % 10) / 10.0f, 1.0f, 1.0f));
				setPrimScale(b, glm::vec3(5, 1.5, 10));
				state.brickPrimHdls[i] = b;
			}

#endif 
		}

		Renderer::populateIndirectCommandBufferForBricks(state.numBricks);
	}

	bool BallIntersectsRect(int rectPrimHdl)
	{
		glm::vec3 rectPos = getPrimPos(rectPrimHdl);
		glm::vec3 rectScale = getPrimScale(rectPrimHdl);

		float left = rectPos.x - rectScale.x;
		float right = rectPos.x + rectScale.x;
		float top = rectPos.y - rectScale.y;
		float bottom = rectPos.y + rectScale.y;

		// Find the closest point to the circle within the rectangle
		float closestX = clamp(state.ballPos.x, left, right);
		float closestY = clamp(state.ballPos.y, top, bottom);

		// Calculate the distance between the circle's center and this closest point
		float distanceX = state.ballPos.x - closestX;
		float distanceY = state.ballPos.y - closestY;

		// If the distance is less than the circle's radius, an intersection occurs
		float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
		return distanceSquared < (state.ballRad * state.ballRad);
	}


	void tick(float deltaTime)
	{
		float aspect = (float)getScreenW() / (float)getScreenH();
		float screenWOrtho = 100.0f * aspect;
		float screenHOrtho = 100.0f;

		//ball movement and intersection
		glm::vec3 frameVel = state.ballVel * deltaTime * 1.0f;

		state.ballPos += frameVel;

		//clamp the ball to the screen / it bounces off edges
		if (state.ballPos.y <= -screenHOrtho + state.ballRad)
		{
			state.ballPos -= frameVel;
			state.ballVel.y *= -1;
		}

		//treat bottom edge separately, it's game over if ball hits it
		if (state.ballPos.y >= screenHOrtho - state.ballRad)
		{
			state.ballPos -= frameVel;
			state.ballVel.y *= -1;
			state.gameOver = true;
		}

		if (state.ballPos.x >= screenWOrtho - state.ballRad
			|| state.ballPos.x <= -screenWOrtho + state.ballRad)
		{
			state.ballPos -= frameVel;
			state.ballVel.x *= -1;
		}

		//check for collision with bricks, kill brick if hit
		for (int i = 0; i < state.numBricks; ++i)
		{
			if (state.brickPrimHdls[i] > -1 && BallIntersectsRect(state.brickPrimHdls[i]))
			{
				glm::vec3 brickPos = getPrimPos(state.brickPrimHdls[i]);
				glm::vec3 brickScale = getPrimScale(state.brickPrimHdls[i]);

				state.ballPos -= state.ballVel * 0.5f;
				setPrimPos(state.ballPrimHdl, state.ballPos);

				state.ballVel.y *= -1;

				destroyPrimitive(state.brickPrimHdls[i]);
				state.brickPrimHdls[i] = -1;
			}
		}

		if (BallIntersectsRect(state.paddlePrimHdl))
		{
			//paddle doesn't get killed on hit, so we need to back up
			//until we *just* aren't intersecting before bouncing
			while (BallIntersectsRect(state.paddlePrimHdl))
			{
				state.ballPos -= state.ballVel * 0.1f;
			}

			float xIntersect = clamp(state.ballPos.x - state.paddlePos.x, -0.5, 0.5);

			if (xIntersect * xIntersect > (0.3 * 0.3))
			{
				state.ballVel.x = xIntersect * 0.15f;
				state.ballVel = glm::normalize(state.ballVel) * 0.1f;
			}

			state.ballVel.y *= -1;
		}

		setPrimPos(state.ballPrimHdl, state.ballPos);


		//move paddle
		if (getKey(KeyCode::KEY_LEFT))
		{
			state.paddlePos -= glm::vec3(0.15f * deltaTime, 0, 0);
		}
		else if (getKey(KeyCode::KEY_RIGHT))
		{
			state.paddlePos += glm::vec3(0.15f * deltaTime, 0, 0);
		}

		state.paddlePos.x = clamp(state.paddlePos.x, -screenWOrtho + state.paddleScale.x, screenWOrtho - state.paddleScale.x);
		setPrimPos(state.paddlePrimHdl, state.paddlePos);
	}

	void draw()
	{
		submitPrimitives();
	}

	bool isGameOver()
	{
		if (state.gameOver) return true;

		for (int i = 0; i < state.numBricks; ++i)
		{
			if (state.brickPrimHdls[i] > -1) return false;
		}

		return true;
	}
}