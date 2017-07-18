#pragma once
#include "vkh.h"
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Renderer
{
public:
	Renderer(uint32_t width, uint32_t height, HINSTANCE Instance, HWND wndHdl, const char* applicationName);
	void draw(const class Primitive* blockPrims, const class Primitive* ballPrims, const class Primitive* paddlePrims);
	vkh::VkhContext& GetVkContext();


	~Renderer();

private:
	void createPipelines();
	void createDescriptorSetLayout();
	void createDescriptorSet();

	vkh::VkhContext context;
	vkh::VkhMaterial blockMaterial;

	//we don't have a camera, so we'll store the ortho matrix here
	glm::mat4 VIEW_PROJECTION;

	VkRenderPass renderPass; //only 1 render pass for this application
	std::vector<VkFramebuffer>	swapChainFramebuffers;

	//the input layout for all shaders used
	VkDescriptorSetLayout		descriptorSetLayout;


	//three options: 
	// 1 - 1 descriptorSet bound to 1 uniform buffer per Object (still have to map to update)
	// 2 - single descriptorSet for whole material, 1 uniform buffer per object (have to updateDescriptorSet multiple times per frame, still have to map UBO)
	// 3 - single descriptor set, single buffer, (have to map data into the reused buffer)
	// which is faster? better? Let's start with #3


#if PER_PRIMITIVE_DESCRIPTOR_SET
#elif PER_MATERIAL_DESCRIPTOR_SET
	VkDescriptorSet				descriptorSet;
#endif

#if PER_PRIMITIVE_UNIFORM_BUFFER
#elif PER_MATERIAL_UNIFORM_BUFFER
	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;
#endif

	
};