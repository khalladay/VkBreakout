#include "Primitive.h"
#include "Mesh.h"
#include "vkh.h"
#include "Renderer.h"

Primitive::Primitive()
{
}

Primitive::~Primitive()
{

}

PrimitiveUniformObject Primitive::GetRenderPrimitiveUniformObject() const
{
	PrimitiveUniformObject prim;
	prim.model = glm::translate(pos) * glm::scale(scale);
	prim.color = col;
	return prim;
}