#pragma once
#include "stdafx.h"
#include <map>

struct PrimitiveUniformObject
{
	glm::mat4 model;
	glm::vec4 color;
};

class PrimitiveManager
{
public:
	static PrimitiveManager* Get();
	
	int NewPrimitive(class Mesh* meshResource);
	void DestroyPrimitive(int handle);

	void SetPrimScale(int hdl, glm::vec3 scale);
	void SetPrimPos(int hdl, glm::vec3 pos);
	void SetPrimCol(int hdl, glm::vec4 col);

	glm::vec3 GetPrimPos(int hdl);
	glm::vec3 GetPrimScale(int hdl);

	//this parses Primitives into the more mappable PrimitiveUniformObject types
	void SubmitPrimitives(class Renderer* renderer);

public:
	struct Primitive
	{
		glm::vec3 pos;
		glm::vec3 scale;
		glm::vec4 col;
	};

private:
	std::map<int, Primitive> primitives;
	std::map<int, class Mesh*> primitiveMeshes;

	PrimitiveUniformObject* uniformData;

	PrimitiveManager();
	~PrimitiveManager();
};

#define PM PrimitiveManager::Get()
