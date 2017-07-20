#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#include "file_utils.h"
#include <vector>
#include "stdafx.h"

#define PER_MATERIAL_DESCRIPTOR_SET 1
//#define PER_PRIMITIVE_DESCRIPTOR_SET 1

#define PER_MATERIAL_UNIFORM_BUFFER 1
//#define PER_PRIMITIVE_UNIFORM_BUFFER 1

namespace vkh
{
	const uint32_t INVALID_QUEUE_FAMILY_IDX = -1;

	struct VkhSurface
	{
		VkSurfaceKHR surface;
		VkFormat format;
	};

	struct VkhSwapChainSupportInfo
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct VkhPhysicalDevice
	{
		VkPhysicalDevice					device;
		VkPhysicalDeviceProperties			deviceProps;
		VkPhysicalDeviceMemoryProperties	memProps;
		VkPhysicalDeviceFeatures			features;
		VkhSwapChainSupportInfo				swapChainSupport;
		uint32_t							queueFamilyCount;
		uint32_t							presentQueueFamilyIdx;
		uint32_t							graphicsQueueFamilyIdx;
		uint32_t							transferQueueFamilyIdx;
	};

	struct VkhSwapChain
	{
		VkSwapchainKHR				swapChain;
		VkFormat					imageFormat;
		VkExtent2D					extent;
		std::vector<VkImage>		imageHandles;
		std::vector<VkImageView>	imageViews;
	};

	struct VkhLogicalDevice
	{
		VkDevice	device;
		VkQueue		graphicsQueue;
		VkQueue		transferQueue;
		VkQueue		presentQueue;
	};

	struct VkhContext
	{
		VkInstance						instance;
		vkh::VkhSwapChain				swapChain;
		vkh::VkhSurface					surface;
		vkh::VkhPhysicalDevice			gpu;
		vkh::VkhLogicalDevice			lDevice;
		VkCommandPool					commandPool;
		std::vector<VkCommandBuffer>	commandBuffers;
		VkDescriptorPool				descriptorPool;
		VkSemaphore						imageAvailableSemaphore;
		VkSemaphore						renderFinishedSemaphore;
		VkFence*						frameFences;

	};

	struct VkhMaterial
	{
		VkPipeline gfxPipeline;
		VkPipelineLayout pipelineLayout;
	};

	//vulkan initialization
	void CreateWindowsInstance(VkInstance& outInstance, const char* applicationName);
	void CreateWin32Context(VkhContext& outContext, uint32_t width, uint32_t height, HINSTANCE Instance, HWND wndHdl, const char* applicationName);
	void DestroyContext(VkhContext& context);
	void RecreateSwapChain(VkhContext& context, std::vector<VkFramebuffer>& frameBuffers, VkRenderPass& renderPass);

	void GetDiscretePhysicalDevice(VkhPhysicalDevice& outDevice, VkInstance& inInstance, const VkhSurface& surface);
	void CreateLogicalDevice(VkhLogicalDevice& outDevice, const VkhPhysicalDevice& physDevice);
	void CreateWin32Surface(VkhSurface& outSurface, VkInstance& vkInstance, HINSTANCE win32Instance, HWND wndHdl);
	void CreateSwapchainForSurface(VkhSwapChain& outSwapChain, VkhPhysicalDevice& physDevice, const VkDevice& lDevice, const VkhSurface& surface);
	void CreateCommandPool(VkCommandPool& outPool, const VkDevice& lDevice, const VkhPhysicalDevice& physDevice);
	void CreateCommandBuffers(std::vector<VkCommandBuffer>& outBuffers, VkCommandPool& pool, uint32_t frameBufferCount, const VkDevice& lDevice);
	void CreateSemaphone(VkSemaphore& outSemaphore, const VkDevice& device);
	void CreateDescriptorPool(VkDescriptorPool& outPool, const VkDevice& device, uint32_t maxDescriptors);
	void DestroyDebugCallback(VkInstance instance);


	void CreateColorOnlyRenderPass(VkRenderPass& outPass, const VkhSwapChain& swapChain, const VkDevice& device);
	void CreateFramebuffers(std::vector<VkFramebuffer>& outBuffers, const VkhSwapChain& swapChain, const VkRenderPass& renderPass, const VkDevice& device);
	void CreateBuffer(VkBuffer& outBuffer, VkDeviceMemory& bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const VkDevice& device, const VkPhysicalDevice& physDevice);

	//pipeline initialization
	void CreateShaderModule(VkShaderModule& outModule, const BinaryBuffer& binaryData, const VkDevice& lDevice);
	void CreateShaderModule(VkShaderModule& outModule, const char* filepath, const VkDevice& lDevice);
	void CreateDefaultViewportForSwapChain(VkViewport& outViewport, const VkhSwapChain& swapChain);

	void CreateOpaqueColorBlendAttachState(VkPipelineColorBlendAttachmentState& outState);
	void CreateDefaultPipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo& outInfo);
	void CreateDefaultColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo& outInfo, const VkPipelineColorBlendAttachmentState& blendState);
	void CreateMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo& outInfo, uint32_t sampleCount);
	void CreateDefaultRenderPassBeginInfo(VkRenderPassBeginInfo& outInfo, VkRenderPass& targetPass, VkFramebuffer& targetBuffer, VkExtent2D extent);

}