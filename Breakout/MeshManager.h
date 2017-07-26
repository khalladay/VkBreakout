#pragma once

class MeshManager
{
public:
	static MeshManager* Get();
	class Mesh* GetRectMesh(class Renderer* renderer);
	class Mesh* GetCircleMesh(class Renderer* renderer);

private:
	MeshManager();
	~MeshManager();

	class Mesh* circleMesh;
	class Mesh* rectMesh;
};