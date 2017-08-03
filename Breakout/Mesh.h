#pragma once
#include "stdafx.h"
#include <array>
#include <vector>
#include "vkh.h"

class Mesh
{
public:

	struct Vertex {
		glm::vec3 pos;

		//how to pass this data to a shader once it's in GPU memory
		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);

			//move to the next value after each vertex
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions = {};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			return attributeDescriptions;
		}
	};

	Mesh(const std::vector<Vertex>& verts);
	Mesh(const std::vector<Vertex>& verts, const std::vector<UINT16> indices);

	~Mesh();

	vkh::VkhContext* rc; //render context

	VkBuffer vBuffer;
	VkDeviceMemory vBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	int vertCount;
	int indexCount;
	int id; 

};

static VkPipelineVertexInputStateCreateInfo DefaultVertexInputStateCreateInfo();