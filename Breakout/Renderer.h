#pragma once
#include "vkh.h"
#include <vector>

#include "stdafx.h"

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

	//we don't need a full camera object, so we'll store the ortho matrix here
	glm::mat4 VIEW_PROJECTION;

	VkRenderPass renderPass; //only 1 render pass for this application
	std::vector<VkFramebuffer>	swapChainFramebuffers;

	//the input layout for all shaders used
	VkDescriptorSetLayout		descriptorSetLayout;
	VkDescriptorSet				descriptorSet;

	//Because this application is simple enough, we use a single uniform buffer for the whole application
	//We can choose between three different ways to use this buffer, for performance testing: 
	//	1. Allocating a buffer large enough to hold just 1 object's uniforms, updating that buffer for each object with VkCmdUpdateBuffer
	//  2. Allocating a buffer large enough to hold all objects' uniforms, and getting offsets into that buffer per primitive
	//  3. Not using this buffer at all, and supplying the data using push constants. 
	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;

	
};