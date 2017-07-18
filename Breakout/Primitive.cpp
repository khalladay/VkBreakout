#include "Primitive.h"
#include "Mesh.h"

Primitive::Primitive()
{

}

Primitive::~Primitive()
{

}

const RenderPrimitive* Primitive::GetRenderPrimitive()
{
	renderPrim.model = glm::translate(pos) * glm::scale(scale);
	renderPrim.color = col;
	return &renderPrim;
}