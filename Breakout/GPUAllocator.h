#pragma once

//simplest allocator possible
class GPUAllocator
{
public:
	GPUAllocator(const VkDevice& device, const VkPhysicalDevice& physDevice);
	~GPUAllocator();
	
	struct VkDeviceMemory* Alloc(size_t size);
	void FreeAll();

private:

	class Renderer* renderer;
	struct VKDeviceMemory* memPool;
};