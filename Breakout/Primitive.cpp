#include "Primitive.h"
#include "Mesh.h"
#include "vkh.h"
#include "Renderer.h"


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

	primitives.erase(handle);
	primitiveMeshes.erase(handle);
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
	else if (bufferSize > lastBufferSize)
	{
		uniformData = (PrimitiveUniformObject*)_aligned_realloc(uniformData, bufferSize, dynamicAlignment);
		lastBufferSize = bufferSize;
	}

	std::vector<Mesh*> meshes;

	int idx = 0;
	char* uniformChar = (char*)uniformData;

	for (const auto& prim : primitives)
	{
		PrimitiveUniformObject puo;
		puo.model = renderer->VIEW_PROJECTION * (glm::translate(prim.second.pos) * glm::scale(prim.second.scale));
		puo.color = prim.second.col;

		memcpy(&uniformChar[idx * dynamicAlignment], &puo, sizeof(PrimitiveUniformObject));
		idx++;
		
		meshes.push_back(primitiveMeshes[prim.first]);
	}

	renderer->draw(uniformData, meshes);
}


int PrimitiveManager::NewPrimitive(Mesh* meshResource)
{
	static int next_prim_id = 0;

	PrimitiveManager::Primitive p;
	p.col = glm::vec4(1, 1, 1, 1);
	p.pos = glm::vec3(0, 0, 0);
	p.scale = glm::vec3(1, 1, 1);

	primitives.emplace(next_prim_id, p);
	primitiveMeshes.emplace(next_prim_id, meshResource);

	next_prim_id++;
	return next_prim_id - 1;
}

void PrimitiveManager::SetPrimScale(int hdl, glm::vec3 scale)
{
	primitives[hdl].scale = scale;
}

void PrimitiveManager::SetPrimPos(int hdl, glm::vec3 pos)
{
	primitives[hdl].pos = pos;
}

glm::vec3 PrimitiveManager::GetPrimPos(int hdl)
{
	return primitives[hdl].pos;
}

glm::vec3 PrimitiveManager::GetPrimScale(int hdl)
{
	return primitives[hdl].scale;
}

void PrimitiveManager::SetPrimCol(int hdl, glm::vec4 col)
{
	primitives[hdl].col = col;
}

PrimitiveManager::PrimitiveManager()
{

}

PrimitiveManager::~PrimitiveManager()
{

}
