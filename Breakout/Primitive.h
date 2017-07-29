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

	//this parses Primitives into the more mappable PrimitiveUniformObject types
	void SubmitPrimitives(class Renderer* renderer);

public:
	struct Primitive
	{
		glm::vec3 pos;
		glm::vec3 scale;
		glm::vec4 col;
	};

	std::map<int, Primitive> primitives;
	std::map<int, Mesh*> primitiveMeshes;

	//std::vector<Primitive> primitives;
	//std::vector<Mesh*> primitiveMeshes;
	PrimitiveUniformObject* uniformData;

private:
	PrimitiveManager();
	~PrimitiveManager();
};

void SetPrimPos(int primHdl, glm::vec3 position);
void TranslatePrim(int primHdl, glm::vec3 translation);
void SetPrimCol(int primHdl, glm::vec4 col);
void SetPrimScale(int primHdl, glm::vec3 scale);