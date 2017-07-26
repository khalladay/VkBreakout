#include "GPUAllocator.h"
#include "vkh.h"

GPUAllocator::GPUAllocator(const VkDevice& device, const VkPhysicalDevice& physDevice)
{
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(physDevice, memRequirements.memoryTypeBits, properties);

}

GPUAllocator::~GPUAllocator()
{

}

VkDeviceMemory* GPUAllocator::Alloc(size_t size)
{
	return nullptr;
}

void GPUAllocator::FreeAll()
{

}