#pragma once
#include "stdafx.h"

struct PrimitiveUniformObject
{
	glm::mat4 model;
	glm::vec4 color;
};

struct Primitive
{
	glm::vec3 pos;
	glm::vec3 scale;
	glm::vec4 col;
	class Mesh* meshResource;
};

int NewPrimitive(class Mesh* meshResource);
void DestroyPrimitive(int handle);

void SetPrimScale(int hdl, glm::vec3 scale);
void SetPrimPos(int hdl, glm::vec3 pos);
void SetPrimCol(int hdl, glm::vec4 col);

glm::vec3 GetPrimPos(int hdl);
glm::vec3 GetPrimScale(int hdl);

void SubmitPrimitives(class Renderer* renderer);

