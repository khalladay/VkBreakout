#include "Renderer.h"
#include "Primitive.h"
#include "Mesh.h"

Renderer::Renderer(uint32_t width, uint32_t height, HINSTANCE Instance, HWND wndHdl, const char* applicationName)
{
	vkh::CreateWin32Context(context, width, height, Instance, wndHdl, applicationName);
	vkh::CreateColorOnlyRenderPass(renderPass, context.swapChain, context.lDevice.device);
	vkh::CreateFramebuffers(swapChainFramebuffers, context.swapChain, renderPass, context.lDevice.device);
	createDescriptorSetLayout();
	createDescriptorSet();
	createPipelines();
	
	float aspect = (float)SCREEN_W / (float)SCREEN_H;
	float invAspect = (float)SCREEN_H / (float)SCREEN_W;
	VIEW_PROJECTION = glm::ortho(-(float)SCREEN_W, (float)SCREEN_W, -(float)SCREEN_H, (float)SCREEN_H, -1.0f, 1.0f);
}

vkh::VkhContext& Renderer::GetVkContext()
{
	return context;
}

void Renderer::draw(const class Primitive* blockPrims, const class Primitive* ballPrims, const class Primitive* paddlePrims)
{
	void* udata;
	vkMapMemory(context.lDevice.device, uniformBufferMemory, 0, sizeof(PrimitiveUniformObject), 0, &udata);
	PrimitiveUniformObject paddleUBO = paddlePrims[0].GetRenderPrimitiveUniformObject();
	paddleUBO.model = VIEW_PROJECTION* paddleUBO.model;
	memcpy(udata, &paddleUBO, sizeof(PrimitiveUniformObject));
	vkUnmapMemory(context.lDevice.device, uniformBufferMemory);

	VkResult res;

	//acquire an image from the swap chain
	uint32_t imageIndex;

	//using uint64 max for timeout disables it
	res = vkAcquireNextImageKHR(context.lDevice.device, context.swapChain.swapChain, UINT64_MAX, context.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	if (context.frameFences[imageIndex])
	{
		// Fence should be unsignaled
		if (vkGetFenceStatus(context.lDevice.device, context.frameFences[imageIndex]) == VK_SUCCESS)
		{
			vkWaitForFences(context.lDevice.device, 1, &context.frameFences[imageIndex], true, 0);
		}
	}
	vkResetFences(context.lDevice.device, 1, &context.frameFences[imageIndex]);

	if (res == VK_ERROR_OUT_OF_DATE_KHR)
	{
//		handleScreenResize();
		return;
	}
	else
	{
		assert(res == VK_SUCCESS);
	}


	//record drawing commands for cmd buffer
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional
	vkResetCommandBuffer(context.commandBuffers[imageIndex], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	res = vkBeginCommandBuffer(context.commandBuffers[imageIndex], &beginInfo);
	assert(res == VK_SUCCESS);

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = context.swapChain.extent;

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(context.commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(context.commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, blockMaterial.gfxPipeline);
	VkBuffer vertexBuffers[] = { paddlePrims[0].meshResource->vBuffer };
	VkDeviceSize offsets[] = { 0 };


	vkCmdBindDescriptorSets(context.commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, blockMaterial.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

	vkCmdBindVertexBuffers(context.commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(context.commandBuffers[imageIndex], paddlePrims[0].meshResource->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(context.commandBuffers[imageIndex], static_cast<uint32_t>(paddlePrims[0].meshResource->indexCount), 1, 0, 0, 0);

	vkCmdEndRenderPass(context.commandBuffers[imageIndex]);

	res = vkEndCommandBuffer(context.commandBuffers[imageIndex]);
	assert(res == VK_SUCCESS);

	//wait on writing colours to the buffer until the semaphore says the buffer is available
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { context.imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &context.commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { context.renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	res = vkQueueSubmit(context.lDevice.graphicsQueue, 1, &submitInfo, context.frameFences[imageIndex]);
	assert(res == VK_SUCCESS);


	//finally, present this thing on screen

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { context.swapChain.swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	res = vkQueuePresentKHR(context.lDevice.presentQueue, &presentInfo);

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
	{
	//	handleScreenResize();  
	}

}

void Renderer::createDescriptorSet()
{
	VkResult res;

	VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = context.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	res = vkAllocateDescriptorSets(context.lDevice.device, &allocInfo, &descriptorSet);

	//now we need to configure the descriptor inside the set.

	//our descriptor points to a uniform buffer, so it's configured with
	//a VkDescriptorBufferInfo
	
#if USE_UNIFORM_BUFFER
	VkDeviceSize bufferSize = sizeof(PrimitiveUniformObject) * 256;
#endif

	vkh::CreateBuffer(uniformBuffer,
		uniformBufferMemory,
		bufferSize,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		context.lDevice.device,
		context.gpu.device);


	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(PrimitiveUniformObject);

	//The configuration of descriptors is updated using the vkUpdateDescriptorSets function, which takes an array of VkWriteDescriptorSet structs as parameter.
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0; //refers to binding in shader
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pImageInfo = nullptr; // Optional
	descriptorWrite.pTexelBufferView = nullptr; // Optional

	vkUpdateDescriptorSets(context.lDevice.device, 1, &descriptorWrite, 0, nullptr);
}

void Renderer::createDescriptorSetLayout()
{
	//we only need a single binding, since we're passing both our params in a single UBO 
	VkDescriptorSetLayoutBinding mvpLayoutBinding = {};
	mvpLayoutBinding.binding = 0;
	mvpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mvpLayoutBinding.descriptorCount = 1;
	mvpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	mvpLayoutBinding.pImmutableSamplers = nullptr; // Optional

	//different bindings NEED different uniform buffers. 
	VkDescriptorSetLayoutBinding bindings[] = { mvpLayoutBinding };

	//a resource descriptor is a way for shaders to freely access resources like buffers and images
	//to use our uniform data, we need to tell Vulkan about our descriptor

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = bindings;

	VkResult res = vkCreateDescriptorSetLayout(context.lDevice.device, &layoutInfo, nullptr, &descriptorSetLayout);
	assert(res == VK_SUCCESS);
}

void Renderer::createPipelines()
{
	//create paddle pipeline
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.pName = "main";
	vkh::CreateShaderModule(vertShaderStageInfo.module, "./shaders/vertColorPassthrough.vert.spv", context.lDevice.device);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.pName = "main";
	vkh::CreateShaderModule(fragShaderStageInfo.module, "./shaders/fragFlatColor.frag.spv", context.lDevice.device);

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//now we need to set up all the fixed function parts of the pipeline
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


	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkViewport viewport;
	vkh::CreateDefaultViewportForSwapChain(viewport, context.swapChain);


	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = context.swapChain.extent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	vkh::CreateDefaultPipelineRasterizationStateCreateInfo(rasterizer);

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	vkh::CreateMultisampleStateCreateInfo(multisampling, 1);

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	vkh::CreateOpaqueColorBlendAttachState(colorBlendAttachment);

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	vkh::CreateDefaultColorBlendStateCreateInfo(colorBlending, colorBlendAttachment);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = 0; // Optional

	VkResult res = vkCreatePipelineLayout(context.lDevice.device, &pipelineLayoutInfo, nullptr, &blockMaterial.pipelineLayout);
	assert(res == VK_SUCCESS);


	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = blockMaterial.pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	//can use this to create new pipelines by deriving from old ones
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	res = vkCreateGraphicsPipelines(context.lDevice.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &blockMaterial.gfxPipeline);
	assert(res == VK_SUCCESS);

	vkDestroyShaderModule(context.lDevice.device, vertShaderStageInfo.module, nullptr);
	vkDestroyShaderModule(context.lDevice.device, fragShaderStageInfo.module, nullptr);

}

Renderer::~Renderer()
{
	vkDestroyPipeline(context.lDevice.device, blockMaterial.gfxPipeline, nullptr);
	vkDestroyPipelineLayout(context.lDevice.device, blockMaterial.pipelineLayout, nullptr);

	vkh::DestroyContext(context);

}