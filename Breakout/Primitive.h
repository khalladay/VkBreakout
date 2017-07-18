#pragma once
#include "vkh.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct RenderPrimitive
{
	glm::mat4 model;
	glm::vec4 color;
};

class Primitive
{
public:
	Primitive();
	~Primitive();

	RenderPrimitive GetRenderPrimitive() const;

	glm::vec3 pos;
	glm::vec4 col;
	glm::vec3 scale;
	class Mesh* meshResource;

private:
	RenderPrimitive renderPrim;
};