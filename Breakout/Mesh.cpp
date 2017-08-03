#include "Mesh.h"
#include "vkh.h"

Mesh::Mesh(const std::vector<Vertex>& verts)
{
	auto bufferSize = sizeof(Vertex) * verts.size();
	vertCount = verts.size();
	vkh::CreateBuffer(vBuffer,
		vBufferMemory,
		bufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vkh::GContext.lDevice.device,
		vkh::GContext.gpu.device);

	void* data;
	vkMapMemory(vkh::GContext.lDevice.device, vBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verts.data(), bufferSize);
	vkUnmapMemory(vkh::GContext.lDevice.device, vBufferMemory);

}

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<UINT16> indices)
{
	//copy vert data
	auto bufferSize = sizeof(Vertex) * verts.size();
	vertCount = verts.size();
	vkh::CreateBuffer(vBuffer,
		vBufferMemory,
		bufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vkh::GContext.lDevice.device,
		vkh::GContext.gpu.device);

	void* data;
	vkMapMemory(vkh::GContext.lDevice.device, vBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verts.data(), bufferSize);
	vkUnmapMemory(vkh::GContext.lDevice.device, vBufferMemory);

	//copy index data
	auto indexBufSize = sizeof(UINT16) * indices.size();
	indexCount = indices.size();
	vkh::CreateBuffer(indexBuffer,
		indexBufferMemory,
		indexBufSize,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vkh::GContext.lDevice.device,
		vkh::GContext.gpu.device);

	void* idata;
	vkMapMemory(vkh::GContext.lDevice.device, indexBufferMemory, 0, indexBufSize, 0, &idata);
	memcpy(idata, indices.data(), indexBufSize);
	vkUnmapMemory(vkh::GContext.lDevice.device, indexBufferMemory);
}

Mesh::~Mesh()
{
	vkDestroyBuffer(vkh::GContext.lDevice.device, vBuffer, nullptr);
	vkFreeMemory(vkh::GContext.lDevice.device, vBufferMemory, nullptr);

	vkDestroyBuffer(vkh::GContext.lDevice.device, indexBuffer, nullptr);
	vkFreeMemory(vkh::GContext.lDevice.device, indexBufferMemory, nullptr);


}

VkPipelineVertexInputStateCreateInfo DefaultVertexInputStateCreateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Mesh::Vertex::getBindingDescription();
	auto attributeDescriptions = Mesh::Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional
	return vertexInputInfo;
}