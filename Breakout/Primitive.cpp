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

PrimitiveManager* PrimitiveManager::Get()
{
	static PrimitiveManager* instance = nullptr;
	if (!instance)
	{
		instance = new PrimitiveManager();
	}

	return instance;
}

void PrimitiveManager::DestroyPrimitive(int handle)
{
	primitives[handle].alive = 0;
}

void PrimitiveManager::SubmitPrimitives(Renderer* renderer)
{
	size_t uboAlignment = renderer->GetVkContext().gpu.deviceProps.limits.minUniformBufferOffsetAlignment;
	size_t dynamicAlignment = (sizeof(PrimitiveUniformObject) / uboAlignment) * uboAlignment + ((sizeof(PrimitiveUniformObject) % uboAlignment) > 0 ? uboAlignment : 0);

	size_t bufferSize = primitives.size() * dynamicAlignment;

	static int lastBufferSize = -1;

	if (!uniformData)
	{
		uniformData = (PrimitiveUniformObject*)_aligned_malloc(bufferSize, dynamicAlignment);
		lastBufferSize = bufferSize;
	}
	else if (bufferSize != lastBufferSize)
	{
		uniformData = (PrimitiveUniformObject*)_aligned_realloc(uniformData, bufferSize, dynamicAlignment);
		lastBufferSize = bufferSize;
	}


	for (int i = 0; i < primitives.size(); ++i)
	{
		PrimitiveUniformObject puo;
		puo.model = renderer->VIEW_PROJECTION * (glm::translate(primitives[i].pos) * glm::scale(primitives[i].scale));
		puo.color = primitives[i].col;
		
		char* uniformChar = (char*)uniformData;
		memcpy(&uniformChar[i * dynamicAlignment], &puo, sizeof(PrimitiveUniformObject));
	}

	renderer->draw(uniformData, primitiveMeshes);
}


int PrimitiveManager::NewPrimitive(Mesh* meshResource)
{
	for (int i = 0; i < primitives.size(); ++i)
	{
		if (primitives[i].alive == 0)
		{
			primitives[i].alive = 1;
			primitives[i].col = glm::vec4(1, 0, 0, 0);
			primitives[i].pos = glm::vec3(0, 0, 0);
			primitives[i].scale = glm::vec3(1, 1, 1);

			primitiveMeshes[i] = meshResource;
			return i;
		}
	}

	Primitive p;
	p.alive = 1;
	p.col = glm::vec4(1, 0, 0, 0);
	p.pos = glm::vec3(0, 0, 0);
	p.scale = glm::vec3(1, 1, 1);

	primitives.push_back(p);
	primitiveMeshes.push_back(meshResource);

	return primitives.size() - 1;
}

PrimitiveManager::PrimitiveManager()
{

}

PrimitiveManager::~PrimitiveManager()
{

}
