#include "Mesh.h"
#include "vkh.h"
#include <map>

struct MeshStore
{
	std::map<int, Mesh> meshes;
};

MeshStore meshData;

int GetRectMesh()
{
	static int rectMeshHdl = -1;
	if (rectMeshHdl < 0)
	{
		const std::vector<Vertex> rectVerts =
		{
			{ { 1.0f, 1.0f, 0.0f } },
			{ { -1.0f, 1.0f, 0.0f } },
			{ { -1.0f, -1.0f, 0.0f } },
			{ { 1.0f, -1.0f, 0.0f } },
		};

		const std::vector<unsigned short> rectIndices =
		{
			0,1,2,2,3,0
		};

		rectMeshHdl = CreateMesh(rectVerts, rectIndices);
	}

	return rectMeshHdl;
	
}


int GetCircleMesh()
{
	static int circleMeshHdl = -1;
	if (circleMeshHdl < 0)
	{
		std::vector<Vertex> circleVerts;
		std::vector<unsigned short> circleIndices;
		circleVerts.push_back(Vertex{ glm::vec3(0, 0, 0) });

		for (int i = 0; i < 360; ++i)
		{
			glm::vec3 vert;
			//   |   /|
			//   |  / |
			//   |-/ <|-- angle i
			//   |/___|
			//  opposite = s=o/h sin(i) * hyp = o;
			//  adj = c=a/h cos(i) * hyp = a;
			//	|hyp| == 1

			vert.x = sinf((float)i);
			vert.y = cosf((float)i);
			vert.z = 0;
			circleVerts.push_back(Vertex{ vert });

			if (i > 0)
			{
				circleIndices.push_back(0);
				circleIndices.push_back(i + 1);
				circleIndices.push_back(i);
			}
		}

		circleMeshHdl = CreateMesh(circleVerts, circleIndices);
	}

	return circleMeshHdl;
}

int CreateMesh(const std::vector<Vertex>& verts, const std::vector<UINT16> indices)
{
	static int id = 0;

	auto bufferSize = sizeof(Vertex) * verts.size();

	Mesh m; 
	m.vertCount = (int)verts.size();
	vkh::CreateBuffer(m.vBuffer,
		m.vBufferMemory,
		bufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vkh::GContext.lDevice.device,
		vkh::GContext.gpu.device);

	void* data;
	vkMapMemory(vkh::GContext.lDevice.device, m.vBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verts.data(), bufferSize);
	vkUnmapMemory(vkh::GContext.lDevice.device, m.vBufferMemory);

	auto indexBufSize = sizeof(UINT16) * indices.size();
	m.indexCount = (int)indices.size();
	vkh::CreateBuffer(m.indexBuffer,
		m.indexBufferMemory,
		indexBufSize,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vkh::GContext.lDevice.device,
		vkh::GContext.gpu.device);

	void* idata;
	vkMapMemory(vkh::GContext.lDevice.device, m.indexBufferMemory, 0, indexBufSize, 0, &idata);
	memcpy(idata, indices.data(), indexBufSize);
	vkUnmapMemory(vkh::GContext.lDevice.device, m.indexBufferMemory);


	meshData.meshes.emplace(id++, m);
	return id - 1;
}

Mesh* GetMeshData(int meshHdl)
{
	return &meshData.meshes[meshHdl];
}

VkVertexInputBindingDescription  getVertexBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);

	//move to the next value after each vertex
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;

}


std::array<VkVertexInputAttributeDescription, 1> getVertexAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions = {};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	return attributeDescriptions;
}


VkPipelineVertexInputStateCreateInfo DefaultVertexInputStateCreateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = getVertexBindingDescription();
	auto attributeDescriptions = getVertexAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional
	return vertexInputInfo;
}

