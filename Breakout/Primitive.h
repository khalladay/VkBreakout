#pragma once
#include "stdafx.h"

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