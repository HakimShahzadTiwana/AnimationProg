#pragma once
#include <Vkbootstrap.h>
#include <vk_mem_alloc.h>

struct VkRenderData {
	VmaAllocator rdAllocator;
	vkb::Instance rdVkbInstance{};
	vkb::Device rbVkbDevice{};
	vkb::Swapchain rdVkbSwapchain{};

	std::vector<VkImage> rdSwapchainImages;
	std::vector<VkImageView> rdSwapchainImageViews;
	std::vector<VkFramebuffer> rdFramebuffers;

	VkFence rdRenderFence = VK_NULL_HANDLE;
	VkSemaphore rdPresentSemaphore = VK_NULL_HANDLE;
	VkCommandBuffer rdCommandBuffer = VK_NULL_HANDLE;
	VkRenderPass rdRenderpass = VK_NULL_HANDLE;
	VkPipeline rdPipeline = VK_NULL_HANDLE;
	VkPipelineLayout rdPipelineLayout = VK_NULL_HANDLE;
	VkDescriptorSet rdDescriptorSet = VK_NULL_HANDLE;
	VkSemaphore rdRenderSemaphore = VK_NULL_HANDLE;
	VkQueue rdGraphicsQueue = VK_NULL_HANDLE;
	VkQueue rdPresentQueue = VK_NULL_HANDLE;
};