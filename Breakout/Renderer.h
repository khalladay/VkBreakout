#pragma once
#include "vkh.h"
#include <vector>
#include "Primitive.h"
#include "stdafx.h"

namespace Renderer
{
	typedef struct
	{
		//the input layout for all shaders used
		VkDescriptorSetLayout		descriptorSetLayout;
		VkDescriptorSet				descriptorSet;
		vkh::VkhMaterial			blockMaterial;

		VkRenderPass renderPass; //only 1 render pass for this application
		std::vector<VkFramebuffer>	swapChainFramebuffers;

		//Because this application is simple enough, we use a 
		//single uniform buffer for the whole application
		VkBuffer uniformBuffer;
		VkDeviceMemory uniformBufferMemory;

		glm::mat4 VIEW_PROJECTION;
		int screenW;
		int screenH;

	}AppRenderData;

	extern AppRenderData appRenderData;

	void initializeRendering(HINSTANCE Instance, HWND wndHdl, const char* applicationName);
	void handleScreenResize(AppRenderData& rd);
	void draw(const Primitive::PrimitiveUniformObject* uniformData, const std::vector<int> primMeshes);

}