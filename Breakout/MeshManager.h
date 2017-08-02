#pragma once

class MeshManager
{
public:
	//a signleton with an initializer function is terrible but for the purposes of the 
	//blog post I'm doing it because I don't care enough to refactor a quickly
	//done breakout clone 
	void Initialize(class Renderer* renderer);

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