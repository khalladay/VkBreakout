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

		VkBuffer uniformBuffer;
		VkDeviceMemory bufferMem;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMem;

		VkDescriptorSet descSet;
		void* mapped;
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
		vkFreeDescriptorSets(vkh::GContext.lDevice.device, vkh::GContext.descriptorPool , 1, &primitiveState.primitives[handle].descSet);
		vkFreeMemory(vkh::GContext.lDevice.device, primitiveState.primitives[handle].bufferMem, nullptr);
		vkDestroyBuffer(vkh::GContext.lDevice.device, primitiveState.primitives[handle].uniformBuffer, nullptr);
		vkUnmapMemory(vkh::GContext.lDevice.device, primitiveState.primitives[handle].bufferMem);

		primitiveState.primitives.erase(handle);
		
	}

	void destroyAllPrimitives()
	{
		for (const auto& prim : primitiveState.primitives)
		{
			vkUnmapMemory(vkh::GContext.lDevice.device, prim.second.bufferMem);

			vkFreeDescriptorSets(vkh::GContext.lDevice.device, vkh::GContext.descriptorPool, 1, &prim.second.descSet);
			vkFreeMemory(vkh::GContext.lDevice.device, prim.second.bufferMem, nullptr);
			vkDestroyBuffer(vkh::GContext.lDevice.device, prim.second.uniformBuffer, nullptr);

		}
		primitiveState.primitives.clear();
	}

	void submitPrimitives()
	{
		using namespace vkh;
		using namespace Renderer;

		std::vector<const VkDescriptorSet*> descSets;
		std::vector<const VkBuffer*> buffers;
		std::vector<int> meshes;

		int idx = 0;
		char* uniformChar = (char*)primitiveState.uniformData;

		for (const auto& prim : primitiveState.primitives)
		{
			PrimitiveUniformObject puo;
			puo.model = Renderer::appRenderData.VIEW_PROJECTION * (glm::translate(prim.second.pos) * glm::scale(prim.second.scale));
			puo.color = prim.second.col;


			static void* udata = nullptr;
			
#if DEVICE_LOCAL_MEMORY
			vkMapMemory(GContext.lDevice.device, prim.second.stagingMem, 0, sizeof(PrimitiveUniformObject), 0, &udata);
			memcpy(udata, &puo, sizeof(PrimitiveUniformObject));
			vkUnmapMemory(GContext.lDevice.device, prim.second.stagingMem);
#else
			vkMapMemory(GContext.lDevice.device, prim.second.bufferMem, 0, sizeof(PrimitiveUniformObject), 0, &udata);
			memcpy(udata, &puo, sizeof(PrimitiveUniformObject));
			vkUnmapMemory(GContext.lDevice.device, prim.second.bufferMem);

#endif

			idx++;
			descSets.push_back(&prim.second.descSet);
			meshes.push_back(prim.second.meshId);

			buffers.push_back(&prim.second.stagingBuffer);
			buffers.push_back(&prim.second.uniformBuffer);
		}
	

		static bool first = false;
		if (!first)
		{
#if DEVICE_LOCAL_MEMORY
			Renderer::recordDrawingCommands(descSets, buffers, meshes);
#else
			Renderer::recordDrawingCommands(descSets, meshes);
#endif
			first = true;
		}

		Renderer::draw();

	}


	int newPrimitive(int meshId)
	{
		static int next_prim_id = 0;

		PrimitiveInstance p;
		p.col = glm::vec4(1, 1, 1, 1);
		p.pos = glm::vec3(0, 0, 0);
		p.scale = glm::vec3(1, 1, 1);
		p.meshId = meshId;

#if DEVICE_LOCAL_MEMORY
		Renderer::createDescriptorSet(p.descSet, p.uniformBuffer, p.bufferMem, p.stagingBuffer, p.stagingMem, Renderer::appRenderData);
		vkMapMemory(vkh::GContext.lDevice.device, p.stagingMem, 0, sizeof(PrimitiveUniformObject), 0, &p.mapped);
#else
		Renderer::createDescriptorSet(p.descSet, p.uniformBuffer, p.bufferMem, Renderer::appRenderData);
		vkMapMemory(vkh::GContext.lDevice.device, p.bufferMem, 0, sizeof(PrimitiveUniformObject), 0, &p.mapped);
#endif

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