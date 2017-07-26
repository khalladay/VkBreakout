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

Mesh* MeshManager::GetCircleMesh(Renderer* renderer)
{
	if (!circleMesh)
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

		circleMesh = new Mesh(rectVerts, rectIndices, renderer);

	}

	return circleMesh;
}

Mesh* MeshManager::GetRectMesh(Renderer* renderer)
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