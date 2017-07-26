#include "vkh.h" 
#include <stdio.h>
#include <vector>
#include <cassert>
#include <fstream>
#include <set>
#include "file_utils.h"
#include <limits>

namespace vkh
{
	VkDebugReportCallbackEXT callback;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData)
	{
		printf("[VALIDATION LAYER] %s \n", msg);
		return VK_FALSE;
	}

	void CreateWin32Context(VkhContext& outContext, uint32_t width, uint32_t height, HINSTANCE Instance, HWND wndHdl, const char* applicationName)
	{
		CreateWindowsInstance(outContext.instance, applicationName);
		CreateWin32Surface(outContext.surface, outContext.instance, Instance, wndHdl);
		GetDiscretePhysicalDevice(outContext.gpu, outContext.instance, outContext.surface);
		CreateLogicalDevice(outContext.lDevice, outContext.gpu);
		CreateSwapchainForSurface(outContext.swapChain, outContext.gpu, outContext.lDevice.device, outContext.surface);
		CreateCommandPool(outContext.commandPool, outContext.lDevice.device, outContext.gpu);
		CreateCommandBuffers(outContext.commandBuffers, outContext.commandPool, outContext.swapChain.imageViews.size(), outContext.lDevice.device);

		CreateDescriptorPool(outContext.descriptorPool, outContext.lDevice.device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 5); //MAX_DESCRIPTORS

		outContext.frameFences = (VkFence*)malloc(sizeof(VkFence) * outContext.swapChain.imageViews.size());
		for (int i = 0; i < outContext.swapChain.imageViews.size(); ++i)
		{
			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.pNext = NULL;
			fenceInfo.flags = 0;
			VkResult vk_res = vkCreateFence(outContext.lDevice.device, &fenceInfo, NULL, &(outContext.frameFences[i]));
			assert(vk_res == VK_SUCCESS);
		}

		CreateSemaphone(outContext.imageAvailableSemaphore, outContext.lDevice.device);
		CreateSemaphone(outContext.renderFinishedSemaphore, outContext.lDevice.device);
	}

	void CreateWindowsInstance(VkInstance& outInstance, const char* applicationName)
	{
		/***********************************/
		/* Set up ApplicationInfo Struct   */
		/***********************************/
		VkApplicationInfo app_info;

		//stype and pnext are the same usage as below
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pNext = NULL;
		app_info.pApplicationName = "VK Cube";
		app_info.applicationVersion = 1;
		app_info.pEngineName = "VK Cube";
		app_info.engineVersion = 1;
		app_info.apiVersion = VK_API_VERSION_1_0;

		std::vector<const char*> validationLayers;
		std::vector<bool> layersAvailable;
		//check for available validation layer
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;

		validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
		layersAvailable.push_back(false);

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


		for (int i = 0; i < validationLayers.size(); ++i)
		{
			for (int j = 0; j < availableLayers.size(); ++j)
			{
				if (strcmp(validationLayers[i], availableLayers[j].layerName) == 0)
				{
					printf("Found validation layers: %s\n", validationLayers[i]);
					layersAvailable[i] = true;
				}

			}
		}

		bool foundAllLayers = true;
		for (int i = 0; i < layersAvailable.size(); ++i)
		{
			foundAllLayers &= layersAvailable[i];
		}

		assert(foundAllLayers);
#endif


		//Get available extensions:
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		printf("Available Vulkan Extensions: \n");

		std::vector<const char*> requiredExtensions;
		requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

		std::vector<bool> extensionsPresent;
		extensionsPresent.push_back(false);
		extensionsPresent.push_back(false);

		if (enableValidationLayers)
		{
			requiredExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			extensionsPresent.push_back(false);
		}



		for (const auto& extension : extensions)
		{
			printf("%s\t", extension.extensionName);

			for (int i = 0; i < requiredExtensions.size(); i++)
			{
				if (strcmp(extension.extensionName, requiredExtensions[i]) == 0)
				{
					printf(" - Enabled");
					extensionsPresent[i] = true;
				}
			}
			printf("\n");
		}

		bool allExtensionsFound = true;
		for (int i = 0; i < 2; i++)
		{
			allExtensionsFound &= extensionsPresent[i];
		}

		assert(allExtensionsFound);

		/***********************************/
		/* Set up InstanceCreateInfo Struct*/
		/***********************************/


		VkInstanceCreateInfo inst_info;

		//useful for driver validation and when passing as void*
		inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		//used to pass extension info when stype is extension defined
		inst_info.pNext = NULL;
		inst_info.flags = 0;
		inst_info.pApplicationInfo = &app_info;
		inst_info.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		inst_info.ppEnabledExtensionNames = requiredExtensions.data();

		//validation layers / other layers
		inst_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		inst_info.ppEnabledLayerNames = validationLayers.data();;


		/********************/
		/* Create VKInstance*/
		/********************/
		VkResult res;
		res = vkCreateInstance(&inst_info, NULL, &outInstance);

		if (res == VK_ERROR_INCOMPATIBLE_DRIVER)
		{
			printf("Driver incompatible");
		}
		else if (res)
		{
			printf("unknown error");
		}

		if (enableValidationLayers)
		{
			VkDebugReportCallbackCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
			createInfo.pfnCallback = debugCallback;

			PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
			CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(outInstance, "vkCreateDebugReportCallbackEXT");

			CreateDebugReportCallback(outInstance, &createInfo, NULL, &callback);
		}

	}

	void CreateWin32Surface(VkhSurface& outSurface, VkInstance& vkInstance, HINSTANCE win32Instance, HWND wndHdl)
	{
		VkWin32SurfaceCreateInfoKHR createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = wndHdl;
		createInfo.hinstance = win32Instance;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		VkResult res = vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &outSurface.surface);
		assert(res == VK_SUCCESS);
	}

	void DestroyDebugCallback(VkInstance instance)
	{
		if (callback)
		{
			PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReporterCallback = VK_NULL_HANDLE;
			DestroyDebugReporterCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

			DestroyDebugReporterCallback(instance, callback, NULL);
		}

	}

	void GetDiscretePhysicalDevice(VkhPhysicalDevice& outDevice, VkInstance& inInstance, const VkhSurface& surface)
	{
		uint32_t gpu_count;
		VkResult res = vkEnumeratePhysicalDevices(inInstance, &gpu_count, NULL);

		// Allocate space and get the list of devices.
		auto gpus = std::vector<VkPhysicalDevice>(gpu_count);
		res = vkEnumeratePhysicalDevices(inInstance, &gpu_count, gpus.data());

		bool found = false;
		int curScore = 0;

		const std::vector<const char*> deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};


		for (const auto& gpu : gpus)
		{
			auto props = VkPhysicalDeviceProperties();
			vkGetPhysicalDeviceProperties(gpu, &props);

			if (props.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				VkPhysicalDeviceProperties deviceProperties;
				VkPhysicalDeviceFeatures deviceFeatures;
				vkGetPhysicalDeviceProperties(gpu, &deviceProperties);
				vkGetPhysicalDeviceFeatures(gpu, &deviceFeatures);

				int score = 1000;
				score += props.limits.maxImageDimension2D;
				score += props.limits.maxFragmentInputComponents;
				score += deviceFeatures.geometryShader ? 1000 : 0;
				score += deviceFeatures.tessellationShader ? 1000 : 0;

				//make sure the device supports presenting

				uint32_t extensionCount;
				vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, nullptr);
				std::vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, availableExtensions.data());

				std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
				for (const auto& extension : availableExtensions)
				{
					requiredExtensions.erase(extension.extensionName);
				}

				bool supportsAllRequiredExtensions = requiredExtensions.empty();
				if (!supportsAllRequiredExtensions) continue;

				//make sure the device supports at least one valid image format for our surface
				VkhSwapChainSupportInfo scSupport;
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface.surface, &scSupport.capabilities);

				uint32_t formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface.surface, &formatCount, nullptr);

				if (formatCount != 0)
				{
					scSupport.formats.resize(formatCount);
					vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface.surface, &formatCount, scSupport.formats.data());
				}
				else
				{
					continue;
				}

				uint32_t presentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface.surface, &presentModeCount, nullptr);

				if (presentModeCount != 0)
				{
					scSupport.presentModes.resize(presentModeCount);
					vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface.surface, &presentModeCount, scSupport.presentModes.data());
				}

				bool worksWithSurface = scSupport.formats.size() > 0 && scSupport.presentModes.size() > 0;

				if (score > curScore && supportsAllRequiredExtensions && worksWithSurface)
				{
					found = true;

					outDevice.device = gpu;
					outDevice.swapChainSupport = scSupport;
					curScore = score;
				}
			}
		}

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(outDevice.device, &deviceProperties);
		printf("Selected GPU: %s\n", deviceProperties.deviceName);

		assert(found);
		assert(res == VK_SUCCESS);

		vkGetPhysicalDeviceFeatures(outDevice.device, &outDevice.features);
		vkGetPhysicalDeviceMemoryProperties(outDevice.device, &outDevice.memProps);
		vkGetPhysicalDeviceProperties(outDevice.device, &outDevice.deviceProps);
		printf("Max mem allocations: %i\n", outDevice.deviceProps.limits.maxMemoryAllocationCount);

		//get queue families while we're here
		vkGetPhysicalDeviceQueueFamilyProperties(outDevice.device, &outDevice.queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(outDevice.queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(outDevice.device, &outDevice.queueFamilyCount, queueFamilies.data());

		// Iterate over each queue to learn whether it supports presenting:
		VkBool32 *pSupportsPresent = (VkBool32 *)malloc(outDevice.queueFamilyCount * sizeof(VkBool32));
		for (uint32_t i = 0; i < outDevice.queueFamilyCount; i++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(outDevice.device, i, surface.surface, &pSupportsPresent[i]);
		}


		outDevice.graphicsQueueFamilyIdx = INVALID_QUEUE_FAMILY_IDX;
		outDevice.transferQueueFamilyIdx = INVALID_QUEUE_FAMILY_IDX;
		outDevice.presentQueueFamilyIdx = INVALID_QUEUE_FAMILY_IDX;

		int i = 0;
		bool foundGfx = false;
		bool foundTransfer = false;
		bool foundPresent = false;

		for (const auto& queueFamily : queueFamilies)
		{
			if (!foundGfx && queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				outDevice.graphicsQueueFamilyIdx = i;
				foundGfx = true;
			}

			if (!foundTransfer && queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				outDevice.transferQueueFamilyIdx = i;
				foundTransfer = true;
			}

			if (!foundPresent && queueFamily.queueCount > 0 && pSupportsPresent[i])
			{
				outDevice.presentQueueFamilyIdx = i;
				foundPresent = true;
			}

			if (foundGfx && foundTransfer && foundPresent) break;

			i++;
		}

		assert(foundGfx && foundPresent && foundTransfer);
	}

	void CreateLogicalDevice(VkhLogicalDevice& outDevice, const VkhPhysicalDevice& physDevice)
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { physDevice.graphicsQueueFamilyIdx, physDevice.transferQueueFamilyIdx, physDevice.presentQueueFamilyIdx };


		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};

			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;

			float queuePriority[] = { 1.0f };
			queueCreateInfo.pQueuePriorities = queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);

		}

		//we don't need anything fancy right now, but this is where you require things
		// like geo shader support

		const std::vector<const char*> deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		std::vector<const char*> validationLayers;

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
		validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");

		//don't do anything else here because we already know the validation layer is available, 
		//else we would have asserted earlier
#endif

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		VkResult res = vkCreateDevice(physDevice.device, &createInfo, nullptr, &outDevice.device);
		assert(res == VK_SUCCESS);

		vkGetDeviceQueue(outDevice.device, physDevice.graphicsQueueFamilyIdx, 0, &outDevice.graphicsQueue);
		vkGetDeviceQueue(outDevice.device, physDevice.transferQueueFamilyIdx, 0, &outDevice.transferQueue);
		vkGetDeviceQueue(outDevice.device, physDevice.presentQueueFamilyIdx, 0, &outDevice.presentQueue);

	}

	void CreateSwapchainForSurface(VkhSwapChain& outSwapChain, VkhPhysicalDevice& physDevice, const VkDevice& lDevice, const VkhSurface& surface)
	{
		//choose the surface format to use
		VkSurfaceFormatKHR desiredFormat;
		VkPresentModeKHR desiredPresentMode;
		VkExtent2D swapExtent;

		bool foundFormat = false;

		//if there is no preferred format, the formats array only contains VK_FORMAT_UNDEFINED
		if (physDevice.swapChainSupport.formats.size() == 1 && physDevice.swapChainSupport.formats[0].format == VK_FORMAT_UNDEFINED)
		{
			desiredFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			desiredFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
			foundFormat = true;
		}

		//otherwise we can't just choose any format we want, but still let's try to grab one that we know will work for us first
		if (!foundFormat)
		{
			for (const auto& availableFormat : physDevice.swapChainSupport.formats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					desiredFormat = availableFormat;
				}
			}
		}

		//if our preferred format isn't available, let's just grab the first available because yolo
		if (!foundFormat)
		{
			desiredFormat = physDevice.swapChainSupport.formats[0];
		}

		//present mode - VK_PRESENT_MODE_MAILBOX_KHR is for triple buffering, VK_PRESENT_MODE_FIFO_KHR is double, VK_PRESENT_MODE_IMMEDIATE_KHR is single
		//VK_PRESENT_MODE_FIFO_KHR  is guaranteed to be available.
		//let's prefer triple buffering, and fall back to double if it isn't supported

		desiredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : physDevice.swapChainSupport.presentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				desiredPresentMode = availablePresentMode;
			}
		}

		//update physdevice for new surface size
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice.device, surface.surface, &physDevice.swapChainSupport.capabilities);

		//swap extent is the resolution of the swapchain
		swapExtent = physDevice.swapChainSupport.capabilities.currentExtent;

		//need 1 more than minimum image count for triple buffering
		uint32_t imageCount = physDevice.swapChainSupport.capabilities.minImageCount + 1;
		if (physDevice.swapChainSupport.capabilities.maxImageCount > 0 && imageCount > physDevice.swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = physDevice.swapChainSupport.capabilities.maxImageCount;
		}

		//now that everything is set up, we need to actually create the swap chain
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface.surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = desiredFormat.format;
		createInfo.imageColorSpace = desiredFormat.colorSpace;
		createInfo.imageExtent = swapExtent;
		createInfo.imageArrayLayers = 1; //always 1 unless a stereoscopic app

										 //here, we're rendering directly to the swap chain, but if we were using post processing, this might be VK_IMAGE_USAGE_TRANSFER_DST_BIT 
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


		if (physDevice.graphicsQueueFamilyIdx != physDevice.presentQueueFamilyIdx)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			uint32_t queueFamilyIndices[] = { physDevice.graphicsQueueFamilyIdx, physDevice.presentQueueFamilyIdx };
			createInfo.pQueueFamilyIndices = queueFamilyIndices;

		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional	
		}

		createInfo.preTransform = physDevice.swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = desiredPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.pNext = NULL;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult res = vkCreateSwapchainKHR(lDevice, &createInfo, nullptr, &outSwapChain.swapChain);
		assert(res == VK_SUCCESS);

		//get images for swap chain
		vkGetSwapchainImagesKHR(lDevice, outSwapChain.swapChain, &imageCount, nullptr);
		outSwapChain.imageHandles.resize(imageCount);
		outSwapChain.imageViews.resize(imageCount);

		vkGetSwapchainImagesKHR(lDevice, outSwapChain.swapChain, &imageCount, outSwapChain.imageHandles.data());

		outSwapChain.imageFormat = desiredFormat.format;
		outSwapChain.extent = swapExtent;

		//create image views
		for (size_t i = 0; i < outSwapChain.imageHandles.size(); i++)
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = outSwapChain.imageHandles[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = outSwapChain.imageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkResult res = vkCreateImageView(lDevice, &createInfo, nullptr, &outSwapChain.imageViews[i]);
			assert(res == VK_SUCCESS);
		}


	}

	void CreateCommandPool(VkCommandPool& outPool, const VkDevice& lDevice, const VkhPhysicalDevice& physDevice)
	{
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = physDevice.graphicsQueueFamilyIdx;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

		VkResult res = vkCreateCommandPool(lDevice, &poolInfo, nullptr, &outPool);
		assert(res == VK_SUCCESS);
	}

	void CreateSemaphone(VkSemaphore& outSemaphore, const VkDevice& device)
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkResult res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &outSemaphore);
		assert(res == VK_SUCCESS);
	}

	void CreateCommandBuffers(std::vector<VkCommandBuffer>& outBuffers, VkCommandPool& pool, uint32_t frameBufferCount, const VkDevice& lDevice)
	{
		outBuffers.resize(frameBufferCount);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = pool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)outBuffers.size();

		VkResult res = vkAllocateCommandBuffers(lDevice, &allocInfo, outBuffers.data());
		assert(res == VK_SUCCESS);

	}

	void CreateShaderModule(VkShaderModule& outModule, const char* filepath, const VkDevice& lDevice)
	{
		BinaryBuffer* data = loadBinaryFile(filepath);
		vkh::CreateShaderModule(outModule, *data, lDevice);
		free(data);
	}

	void CreateShaderModule(VkShaderModule& outModule, const BinaryBuffer& binaryData, const VkDevice& lDevice)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = binaryData.size;

		//data for vulkan is stored in uint32_t  -  so we have to temporarily copy it to a container that respects that alignment

		std::vector<uint32_t> codeAligned(binaryData.size / sizeof(uint32_t) + 1);
		memcpy(codeAligned.data(), binaryData.data, binaryData.size);
		createInfo.pCode = codeAligned.data();

		VkResult res = vkCreateShaderModule(lDevice, &createInfo, nullptr, &outModule);
		assert(res == VK_SUCCESS);
	}

	void CreateColorOnlyRenderPass(VkRenderPass& outPass, const VkhSwapChain& swapChain, const VkDevice& device)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = swapChain.imageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		//we can keep the initial layout as undefined (likely at a performance penalty), since it means
		//the contents in the buffer are undefined...but we're clearing it anyway
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //what to transition to after pass

																	   //The index of the attachment in this array is directly referenced from the fragment shader with the layout(location = 0) out vec4 outColor directive!
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		//we need a subpass dependency for transitioning the image to the right format, because by default, vulkan
		//will try to do that before we have acquired an image from our fb
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL; //External means outside of the render pipeline, in srcPass, it means before the render pipeline
		dependency.dstSubpass = 0; //must be higher than srcSubpass
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		//add the dependency to the renderpassinfo
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult res = vkCreateRenderPass(device, &renderPassInfo, nullptr, &outPass);
		assert(res == VK_SUCCESS);

	}

	void CreateFramebuffers(std::vector<VkFramebuffer>& outBuffers, const VkhSwapChain& swapChain, const VkRenderPass& renderPass, const VkDevice& device)
	{
		outBuffers.clear();
		outBuffers.resize(swapChain.imageViews.size());

		for (size_t i = 0; i < outBuffers.size(); i++)
		{
			VkImageView attachments[] =
			{
				swapChain.imageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChain.extent.width;
			framebufferInfo.height = swapChain.extent.height;
			framebufferInfo.layers = 1;

			VkResult r = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &outBuffers[i]);
			assert(r == VK_SUCCESS);
		}
	}

	void CreateDefaultViewportForSwapChain(VkViewport& outViewport, const VkhSwapChain& swapChain)
	{
		outViewport = {};
		outViewport.x = 0.0f;
		outViewport.y = 0.0f;
		outViewport.width = (float)swapChain.extent.width;
		outViewport.height = (float)swapChain.extent.height;
		outViewport.minDepth = 0.0f;
		outViewport.maxDepth = 1.0f;
	}

	void CreateOpaqueColorBlendAttachState(VkPipelineColorBlendAttachmentState& outState)
	{
		outState = {};
		outState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		outState.blendEnable = VK_FALSE;
		outState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		outState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		outState.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		outState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		outState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		outState.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
	}

	void CreateDefaultPipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo& outInfo)
	{
		outInfo = {};
		outInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		outInfo.depthClampEnable = VK_FALSE; //if values are outside depth, they are clamped to min/max
		outInfo.rasterizerDiscardEnable = VK_FALSE; //this disables output to the fb
		outInfo.polygonMode = VK_POLYGON_MODE_FILL;
		outInfo.lineWidth = 1.0f;
		outInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		outInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		outInfo.depthBiasEnable = VK_FALSE;
		outInfo.depthBiasConstantFactor = 0.0f; // Optional
		outInfo.depthBiasClamp = 0.0f; // Optional
		outInfo.depthBiasSlopeFactor = 0.0f; // Optional
	}

	void CreateMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo& outInfo, uint32_t sampleCount)
	{
		if (sampleCount != 1) printf("WARNING: CreateMultiSampleStateCreateInfo cannot support a sample count that isn't 1 right now\n");

		outInfo = {};
		outInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		outInfo.sampleShadingEnable = VK_FALSE;
		outInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		outInfo.minSampleShading = 1.0f; // Optional
		outInfo.pSampleMask = nullptr; // Optional
		outInfo.alphaToCoverageEnable = VK_FALSE; // Optional
		outInfo.alphaToOneEnable = VK_FALSE; // Optional


	}

	void CreateDefaultColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo& outInfo, const VkPipelineColorBlendAttachmentState& blendState)
	{
		outInfo = {};
		outInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		outInfo.logicOpEnable = VK_FALSE; //can be used for bitwise blending
		outInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
		outInfo.attachmentCount = 1;
		outInfo.pAttachments = &blendState;
		outInfo.blendConstants[0] = 0.0f; // Optional
		outInfo.blendConstants[1] = 0.0f; // Optional
		outInfo.blendConstants[2] = 0.0f; // Optional
		outInfo.blendConstants[3] = 0.0f; // Optional

	}

	//gpus can differ in types of memory to allocate based on operations needed and perf characteristics
	//this function lets us find the right type of memory for our application's needs
	uint32_t FindMemoryType(const VkPhysicalDevice& device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

		//The VkPhysicalDeviceMemoryProperties structure has two arraysL memoryTypes and memoryHeaps.
		//Memory heaps are distinct memory resources like dedicated VRAM and swap space in RAM 
		//for when VRAM runs out.The different types of memory exist within these heaps.Right now 
		//we'll only concern ourselves with the type of memory and not the heap it comes from, 
		//but you can imagine that this can affect performance.

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
			{
				return i;
			}
		}

		assert(0);
		return 0;
	}

	void CreateBuffer(VkBuffer& outBuffer, VkDeviceMemory& outBufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const VkDevice& device, const VkPhysicalDevice& physDevice)
	{
		VkBufferCreateInfo bufferInfo = {};

		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult res = vkCreateBuffer(device, &bufferInfo, nullptr, &outBuffer);
		assert(res == VK_SUCCESS);

		//buffer is allocated but doesn't have anything assigned to it yet

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, outBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(physDevice, memRequirements.memoryTypeBits, properties);

		res = vkAllocateMemory(device, &allocInfo, nullptr, &outBufferMemory);
		assert(res == VK_SUCCESS);

		res = vkBindBufferMemory(device, outBuffer, outBufferMemory, 0);
		assert(res == VK_SUCCESS);

	}

	void CreateDescriptorPool(VkDescriptorPool& outPool, const VkDevice& device, VkDescriptorType descriptorType, uint32_t maxDescriptors)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = descriptorType;
		poolSize.descriptorCount = maxDescriptors;

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = maxDescriptors;

		VkResult res = vkCreateDescriptorPool(device, &poolInfo, nullptr, &outPool);
		assert(res == VK_SUCCESS);
	}

	void CleanupSwapChain(VkhContext& GVK, std::vector<VkFramebuffer>& frameBuffers, VkRenderPass& renderPass)
	{
		for (size_t i = 0; i < frameBuffers.size(); i++)
		{
			vkDestroyFramebuffer(GVK.lDevice.device, frameBuffers[i], nullptr);
		}

		vkFreeCommandBuffers(GVK.lDevice.device, GVK.commandPool, static_cast<uint32_t>(GVK.commandBuffers.size()), GVK.commandBuffers.data());
		//vkDestroyPipeline(GVK.lDevice.device, GVK.graphicsPipeline, nullptr);
		//vkDestroyPipelineLayout(GVK.lDevice.device, GVK.pipelineLayout, nullptr);
		vkDestroyRenderPass(GVK.lDevice.device, renderPass, nullptr);

		for (size_t i = 0; i < GVK.swapChain.imageViews.size(); i++)
		{
			vkDestroyImageView(GVK.lDevice.device, GVK.swapChain.imageViews[i], nullptr);
		}


		vkDestroySwapchainKHR(GVK.lDevice.device, GVK.swapChain.swapChain, nullptr);

	}

	void DestroyContext(VkhContext& context)
	{
		vkDestroyDescriptorPool(context.lDevice.device, context.descriptorPool, nullptr);
		vkDestroySemaphore(context.lDevice.device, context.renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(context.lDevice.device, context.imageAvailableSemaphore, nullptr);

		vkFreeCommandBuffers(context.lDevice.device, context.commandPool, static_cast<uint32_t>(context.commandBuffers.size()), context.commandBuffers.data());
	//	vkDestroyPipeline(context.lDevice.device, context.graphicsPipeline, nullptr);
		//vkDestroyPipelineLayout(context.lDevice.device, context.pipelineLayout, nullptr);

		for (size_t i = 0; i < context.swapChain.imageViews.size(); i++)
		{
			vkDestroyFence(context.lDevice.device, context.frameFences[i], nullptr);
			vkDestroyImageView(context.lDevice.device, context.swapChain.imageViews[i], nullptr);
		}


		vkDestroySwapchainKHR(context.lDevice.device, context.swapChain.swapChain, nullptr);



		vkDestroyCommandPool(context.lDevice.device, context.commandPool, nullptr);

		vkDestroySurfaceKHR(context.instance, context.surface.surface, nullptr);
		vkh::DestroyDebugCallback(context.instance);
		vkDestroyDevice(context.lDevice.device, nullptr);
		vkDestroyInstance(context.instance, nullptr);


	}

	void RecreateSwapChain(VkhContext& context, std::vector<VkFramebuffer>& frameBuffers, VkRenderPass& renderPass)
	{
		CleanupSwapChain(context, frameBuffers, renderPass);

		vkh::CreateSwapchainForSurface(context.swapChain, context.gpu, context.lDevice.device, context.surface);
		vkh::CreateCommandBuffers(context.commandBuffers, context.commandPool, context.swapChain.imageViews.size(), context.lDevice.device);

	}

	void CreateDefaultRenderPassBeginInfo(VkRenderPassBeginInfo& outInfo, VkRenderPass& targetPass, VkFramebuffer& targetBuffer, VkExtent2D extent)
	{
		outInfo = {};
		outInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		outInfo.renderPass = targetPass;
		outInfo.framebuffer = targetBuffer;
		outInfo.renderArea.offset = { 0, 0 };
		outInfo.renderArea.extent = extent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		outInfo.clearValueCount = 1;
		outInfo.pClearValues = &clearColor;

	}
}