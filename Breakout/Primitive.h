#pragma once
#include "stdafx.h"
#include <vector>


struct PrimitiveUniformObject
{
	glm::mat4 model;
	glm::vec4 color;
};


class Primitive
{
public:
	Primitive();
	~Primitive();

	PrimitiveUniformObject GetRenderPrimitiveUniformObject() const;

	glm::vec3 pos;
	glm::vec4 col;
	glm::vec3 scale;
	class Mesh* meshResource;


private:
	PrimitiveUniformObject renderPrim;
};

class PrimitiveManager
{
public:
	static PrimitiveManager* Get();
	
	int NewPrimitive(class Mesh* meshResource);
	void DestroyPrimitive(int handle);

	//this parses Primitives into the more mappable PrimitiveUniformObject types
	void SubmitPrimitives(class Renderer* renderer);

public:
	struct Primitive
	{
		int alive;
		glm::vec3 pos;
		glm::vec3 scale;
		glm::vec4 col;
	};

	std::vector<Primitive> primitives;
	std::vector<Mesh*> primitiveMeshes;
	//std::vector<PrimitiveUniformObject> uniformData;

	PrimitiveUniformObject* uniformData;

private:
	PrimitiveManager();
	~PrimitiveManager();
};