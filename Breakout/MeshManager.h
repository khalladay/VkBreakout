#pragma once

class MeshManager
{
public:

	static MeshManager* Get();
	class Mesh* GetRectMesh();
	class Mesh* GetCircleMesh();

private:
	MeshManager();
	~MeshManager();

	class Mesh* circleMesh;
	class Mesh* rectMesh;
	class Renderer* renderer;
};