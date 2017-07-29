#include "BreakoutGame.h"
#include "os_support.h"
#include "Renderer.h"
#include "stdafx.h"
#include "Primitive.h"
#include "MeshManager.h"

BreakoutGame::BreakoutGame(Renderer* r)
{
	paddlePos = glm::vec3(0, 85,0);
	ballPos = glm::vec2(0, 49);
	ballVel = glm::vec2(0.5, 1);
	paddleVel = glm::vec2(0, 0);
	renderer = r;

	paddlePrimHdl = PrimitiveManager::Get()->NewPrimitive(MeshManager::Get()->GetRectMesh(r));
	PrimitiveManager::Get()->primitives[paddlePrimHdl].pos = paddlePos;
	PrimitiveManager::Get()->primitives[paddlePrimHdl].scale = glm::vec3(10,3,10);

	//construct brick field
	for (int i = 0; i < 50; ++i)
	{
		int b = PrimitiveManager::Get()->NewPrimitive(MeshManager::Get()->GetRectMesh(r));
		glm::vec3 p = glm::vec3(-110 + (i %10) * 25, -20 + (i/10) * 10,0);
		PrimitiveManager::Get()->primitives[b].pos = p;
		PrimitiveManager::Get()->primitives[b].col = glm::vec4( (i / 10) / 5.0f, (i%10) /10.0f, 1.0f, 1.0f);

		PrimitiveManager::Get()->primitives[b].scale = glm::vec3(10, 3, 10);
		brickPrimHdls.push_back(b);
		brickPos.push_back(p);
	}
}

BreakoutGame::~BreakoutGame()
{

}

void BreakoutGame::tick(float deltaTime)
{

}

void BreakoutGame::draw() const
{
	PrimitiveManager::Get()->SubmitPrimitives(renderer);
}

bool BreakoutGame::isGameOver() const
{
	return brickPos.size() > 0;
}

void BreakoutGame::restart()
{

}