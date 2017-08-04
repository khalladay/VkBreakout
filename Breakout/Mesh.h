#pragma once
#include "stdafx.h"
#include <array>
#include <vector>
#include "vkh.h"

struct Vertex 
{
	glm::vec3 pos;
};

VkVertexInputBindingDescription  getVertexBindingDescription();
std::array<VkVertexInputAttributeDescription, 1> getVertexAttributeDescriptions();

struct Mesh
{
	VkBuffer vBuffer;
	VkDeviceMemory vBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	int vertCount;
	int indexCount;
};


int GetRectMesh();
int GetCircleMesh();
int CreateMesh(const std::vector<Vertex>& verts, const std::vector<UINT16> indices);
Mesh* GetMeshData(int meshHdl);


VkPipelineVertexInputStateCreateInfo DefaultVertexInputStateCreateInfo();