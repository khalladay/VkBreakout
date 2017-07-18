#include "Primitive.h"
#include "Mesh.h"

Primitive::Primitive()
{

}

Primitive::~Primitive()
{

}

RenderPrimitive Primitive::GetRenderPrimitive() const
{
	RenderPrimitive prim;
	prim.model = glm::translate(pos) * glm::scale(scale);
	prim.color = col;
	return prim;
}