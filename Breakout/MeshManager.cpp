#include "MeshManager.h"
#include <vector>
#include "Mesh.h"
#include "Renderer.h"

MeshManager::MeshManager()
{

}

MeshManager::~MeshManager()
{

}

MeshManager* MeshManager::Get()
{
	static MeshManager* instance = nullptr;
	if (!instance)
	{
		instance = new MeshManager();
	}
	return instance;
}

void MeshManager::Initialize(Renderer* inR)
{
	renderer = inR;
}


Mesh* MeshManager::GetCircleMesh()
{
	if (!circleMesh)
	{
		std::vector<Mesh::Vertex> circleVerts;
		std::vector<unsigned short> circleIndices;
		circleVerts.push_back(Mesh::Vertex{ glm::vec3(0, 0, 0) });

		for (int i = 0; i < 360; ++i)
		{
			glm::vec3 vert;
			//   |   /|
			//   |  / |
			//   |-/ <|-- angle i
			//   |/___|
			//  opposite = s=o/h sin(i) * hyp = o;
			//  adj = c=a/h cos(i) * hyp = a;
			//	|hyp| == 1

			vert.x = sin(i);
			vert.y = cos(i);
			vert.z = 0;
			circleVerts.push_back(Mesh::Vertex{ vert });

			if (i > 0)
			{
				circleIndices.push_back(0);
				circleIndices.push_back(i + 1);
				circleIndices.push_back(i);
			}
		}

		circleMesh = new Mesh(circleVerts, circleIndices, renderer);
	}

	return circleMesh;
}

Mesh* MeshManager::GetRectMesh()
{
	if (!rectMesh)
	{
		const std::vector<Mesh::Vertex> rectVerts = 
		{
			{ { 1.0f, 1.0f, 0.0f } },
			{ { -1.0f, 1.0f, 0.0f } },
			{ { -1.0f, -1.0f, 0.0f } },
			{ { 1.0f, -1.0f, 0.0f } },
		};

		const std::vector<unsigned short> rectIndices =
		{
			0,1,2,2,3,0
		};

		rectMesh = new Mesh(rectVerts, rectIndices, renderer);

	}

	return rectMesh;
}