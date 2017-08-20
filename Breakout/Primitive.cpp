#include "Primitive.h"

#include "Mesh.h"
#include "vkh.h"
#include "Renderer.h"
#include <map>

namespace Primitive
{
	struct PrimitiveInstance
	{
		glm::vec3 pos;
		glm::vec3 scale;
		glm::vec4 col;
		int meshId;
	};

	struct PrimitiveGameState
	{
		std::map<int, PrimitiveInstance> primitives;
		PrimitiveUniformObject* uniformData;
	};

	static PrimitiveGameState primitiveState;



	void destroyPrimitive(int handle)
	{
		primitiveState.primitives.erase(handle);
	}

	void destroyAllPrimitives()
	{
		primitiveState.primitives.clear();
	}

	void submitPrimitives()
	{
		size_t uboAlignment = vkh::GContext.gpu.deviceProps.limits.minUniformBufferOffsetAlignment;
		size_t dynamicAlignment = (sizeof(PrimitiveUniformObject) / uboAlignment) * uboAlignment + ((sizeof(PrimitiveUniformObject) % uboAlignment) > 0 ? uboAlignment : 0);

		size_t bufferSize = primitiveState.primitives.size() * dynamicAlignment;

		static int lastBufferSize = -1;

		if (!primitiveState.uniformData)
		{
			primitiveState.uniformData = (PrimitiveUniformObject*)malloc(bufferSize);
			lastBufferSize = (int)bufferSize;
		}
		else if (bufferSize > lastBufferSize)
		{
			primitiveState.uniformData = (PrimitiveUniformObject*)realloc(primitiveState.uniformData, bufferSize);
			lastBufferSize = (int)bufferSize;
		}

		std::vector<int> meshes;

		int idx = 0;
		char* uniformChar = (char*)primitiveState.uniformData;

		for (const auto& prim : primitiveState.primitives)
		{
			PrimitiveUniformObject puo;
			puo.model = Renderer::appRenderData.VIEW_PROJECTION * (glm::translate(prim.second.pos) * glm::scale(prim.second.scale));
			puo.color = prim.second.col;

			memcpy(&uniformChar[idx * dynamicAlignment], &puo, sizeof(PrimitiveUniformObject));
			idx++;

			meshes.push_back(prim.second.meshId);
		}
	
		Renderer::draw(primitiveState.uniformData, meshes);

	}


	int newPrimitive(int meshId)
	{
		static int next_prim_id = 0;

		PrimitiveInstance p;
		p.col = glm::vec4(1, 1, 1, 1);
		p.pos = glm::vec3(0, 0, 0);
		p.scale = glm::vec3(1, 1, 1);
		p.meshId = meshId;

		primitiveState.primitives.emplace(next_prim_id, p);

		next_prim_id++;
		return next_prim_id - 1;
	}

	void setPrimScale(int hdl, glm::vec3 scale)
	{
		primitiveState.primitives[hdl].scale = scale;
	}

	void setPrimPos(int hdl, glm::vec3 pos)
	{
		primitiveState.primitives[hdl].pos = pos;
	}

	glm::vec3 getPrimPos(int hdl)
	{
		return primitiveState.primitives[hdl].pos;
	}

	glm::vec3 getPrimScale(int hdl)
	{
		return primitiveState.primitives[hdl].scale;
	}

	void setPrimCol(int hdl, glm::vec4 col)
	{
		primitiveState.primitives[hdl].col = col;
	}
}