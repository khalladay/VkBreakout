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
		vkh::VkhMaterial			blockMaterial;

		VkRenderPass renderPass; //only 1 render pass for this application
		std::vector<VkFramebuffer>	swapChainFramebuffers;

		//for performacne testing
		VkQueryPool					queryPool;

		glm::mat4 VIEW_PROJECTION;
		int screenW;
		int screenH;

	}AppRenderData;

	extern AppRenderData appRenderData;

	void createDescriptorSet(VkDescriptorSet& outDescSet, VkBuffer& outBuffer, VkDeviceMemory& outMemory, AppRenderData& rs);

	void initializeRendering(HINSTANCE Instance, HWND wndHdl, const char* applicationName);
	void handleScreenResize(AppRenderData& rd);
	void draw(const struct Primitive::PrimitiveUniformObject* uniformData, const std::vector<int> primMeshes);

}