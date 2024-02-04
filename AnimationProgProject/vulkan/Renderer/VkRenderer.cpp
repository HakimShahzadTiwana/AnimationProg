#include "VkRenderer.h"
#include "../../logger/Logger.h"

void VkRenderer::uploadData(OGLMesh vertexData)
{
}

bool VkRenderer::draw()
{
	// Wait for fences (these are basically delegates that are called when an async command that was sent to the command buffer has been completed. (GPU to CPU syncronization mechanism))
	// Params: The logical device, count of fences, address of fence obj (can be a vector.data() if multiple), wait for all (true) or wait for atleast one (false), time to wait in nano seconds (max is a long time even in nanoseconds)
	if (vkWaitForFences(mRenderData.rbVkbDevice.device, 1, &mRenderData.rdRenderFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
	
		Logger::log(0, "%s: Error - Failed to wait for fence.\n", __FUNCTION__);
		return false;
	}
	// Reset the fence to an unsignaled state
	if (vkResetFences(mRenderData.rbVkbDevice.device, 1, &mRenderData.rdRenderFence) != VK_SUCCESS) {

		Logger::log(0, "%s: Error - Failed to reset fence.\n", __FUNCTION__);
		return false;
	}

	// Get the next free image from the swapchain( manages multiple images so that atleast one is available for drawing while one is shown via VkSurface object to the user)
	uint32_t imageIndex = 0;
	// Params: Logical device, the swapchain, time to wait for the next image from the swapchain, semaphore used for GPU internal synchronization - signaled as soon as at least one free swapchain image is available, fence to let the call wait for an instuction from the command buffer, out param for index of next available image in swapchain
	VkResult result = vkAcquireNextImageKHR(mRenderData.rbVkbDevice.device, mRenderData.rdVkbSwapchain.swapchain, UINT64_MAX, mRenderData.rdPresentSemaphore, VK_NULL_HANDLE, &imageIndex);

	// Out of date means image differ too much from vulkan surface properties (could be due to window resize etc)
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		// destroys and recreate swapchain with paramters from the surface
		return recreateSwapchain(mRenderData);
	}
	else
	{
		// Suboptimal means that the window has been changed but still the images can be displayed
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			Logger::log(0, "%s: Error - Failed to acquire swapchain image. Error is '%i'\n", __FUNCTION__, result);
			return false;
		}
	}

	// Once swap chain is available we start the command buffer prep so clear the buffer
	if (vkResetCommandBuffer(mRenderData.rdCommandBuffer, 0) != VK_SUCCESS) {
		Logger::log(0, "%s Error - Failed to reset the command buffer. \n", __FUNCTION__);
		return false;
	}

	// Record the commands to send
	VkCommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	// Commands will only be set once and buffer will be reset after the usage (Also possible to create a reusable command buffer object which could be submitted multiple times to the queue)
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	// Function to begin command buffer
	if (vkBeginCommandBuffer(mRenderData.rdCommandBuffer, &cmdBeginInfo) != VK_SUCCESS) {
		Logger::log(0, "%s: Error - Failed to begin Command buffer.\n", __FUNCTION__);
		return false;
	}

	// Init some helper variables
	VkClearValue colorClearValue;

	// Can either store a color value or a depth and stencil type
	colorClearValue.color = { {0.1f,0.1f,0.1f,1.0f} };

	VkClearValue depthValue;

	// Depth ranges from 0 to 1 and is normalized in the pipeline. A small depth value lets the color value pass, a large value discards pixel color because its hidden behind other pixels
	// A stencil is like a silhouette, during a render pass the stencil can be updated by setting a value if a color attachment is written.(Stencil not set here)
	depthValue.depthStencil.depth = 1.0f;

	VkClearValue clearValues[] = { colorClearValue, depthValue };

	VkRenderPassBeginInfo rpInfo{};

	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	// Reads data from the renderpass object
	rpInfo.renderPass = mRenderData.rdRenderpass;
	
	// Origin of image as offset
	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	// Size of image?
	rpInfo.renderArea.extent = mRenderData.rdVkbSwapchain.extent;

	// The framebuffer image that was available from the nextimage call
	rpInfo.framebuffer = mRenderData.rdFramebuffers[imageIndex];
	rpInfo.clearValueCount = 2;
	rpInfo.pClearValues = clearValues;

	// To support dynamic window size we need viewport and scissor object
	// Viewport defines the mapping of the internal Vulkan coordinates to the window coordinates
	// 0,0 is the top-left corner in vulkan (bottom left in opengl)
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;

	// Height and width are identical to the window dimensions, so values are taken from the swapchain which are updated if the window is resized
	viewport.width = static_cast<float>(mRenderData.rdVkbSwapchain.extent.width);
	viewport.height = static_cast<float>(mRenderData.rdVkbSwapchain.extent.height);

	// Give full depth range to the viewport
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Defines a rectangular part of the drawing. If not adjusted will get blank areas around the prev size;
	VkRect2D scissor{};
	scissor.offset = {0,0};
	scissor.extent = mRenderData.rdVkbSwapchain.extent;

	// A Render pass collects all buffers, pipelines and descriptor sets used to render the image
	// Params: The command buffer object (The following commands are recorded here), RenderPassBeginInfo specifies which render pass and framebuffer to use, Defines usage of subpasses inside the render pass telling the render pass to use only a primary command buffer
	vkCmdBeginRenderPass(mRenderData.rdCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Bind rendering pipeline
	// Params: The command buffer, Pipeline usage: draw graphics (can be compute or raytracing pipelines), Pipeline object that contains settings that vulkan needs to know to start rendering process
	vkCmdBindPipeline(mRenderData.rdCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderData.rdPipeline);

	// To use the vertices from a vertex array in the GPU we have to bind the buffer containing vertex data.
	VkDeviceSize offset = 0;

	// Bindings are additional properties of the pipeline which are not immutable to select different slots in the shaders
	// Params: Record this to command buffer as selection of vertex is also a command, binding number, total number of bindings to use,the offset from which part of the buffer we want to start drawing (in our case we want to start from the beginning)
	// The pipeline will now where to find the vertex data for triangles we want to draw
	vkCmdBindVertexBuffers(mRenderData.rdCommandBuffer, 0, 1, &mVertexBuffer, &offset);


	// Binds descriptor set to out command buffer. Descriptor set contains the required information about the vulkan image that we want to use
	// Params: Command buffer, Choose to Draw Graphics in the pipeline again, set the pipeline layout (helper object collecting info related to the pipeline), descriptor number, count of descriptors, count of dynamic buffers, pointer to dynamic buffers (Allows us to bind one large buffer with different model matrix data and choose offset into the buffer at draw time whereas the alternative would be to use a lot of smaller buffers with one matrix eac, depends on architecture and performance needs) 
	vkCmdBindDescriptorSets(mRenderData.rdCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderData.rdPipelineLayout, 0, 1, &mRenderData.rdDescriptorSet, 0, nullptr);

	// Bind the viewport and scissor to the command buffer as well
	vkCmdSetViewport(mRenderData.rdCommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(mRenderData.rdCommandBuffer, 0, 1, &scissor);

	// Command to draw the triangles from the vertex buffer into the framebuffer defined in the rendering pass using the pipelines with the shaders and the desriptor set set with the Vulkan image as the texture.
	// Params: The command buffer, The number of vertices to draw, the number of instances to draw, the first vertex to draw , the first instance to draw
	vkCmdDraw(mRenderData.rdCommandBuffer, mTriangleCount * 3, 1, 0, 0);

	// After drawing end the renderpass
	vkCmdEndRenderPass(mRenderData.rdCommandBuffer);

	// End the command buffer
	if (vkEndCommandBuffer(mRenderData.rdCommandBuffer) != VK_SUCCESS) {
		Logger::log(0, "%s: Error - Failed to end command buffer\n", __FUNCTION__);
		return false;
	}

	// Submit the command buffers to queues of the logical vulkan device (graphic queue of GPU in this case) to start the rendering
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// Define a wwait stage and set it to submit info
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submitInfo.pWaitDstStageMask = &waitStage;

	// Two semaphores needed to coordinate the drawing process in GPU
	// Wait semaphore will block the vulkan rendering before the given wait stage (before reaching the stage where the color attachments of the framebuffer are written, its used int the acquire next image function and informs the rendering process that the previously used image of the swapchain is now free and rendering to the frame buffer can start)
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &mRenderData.rdPresentSemaphore;

	// signal Semaphore does the synchronization by signalling the end of the buffer execution and another vulkan function can be blocked with this semaphore until all commands have been worked on.. (Used by the Queue present function)
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &mRenderData.rdRenderSemaphore;

	submitInfo.commandBufferCount = 1; 
	submitInfo.pCommandBuffers = &mRenderData.rdCommandBuffer;

	// Submit to graphics queue
	if (vkQueueSubmit(mRenderData.rdGraphicsQueue, 1, &submitInfo, mRenderData.rdRenderFence) != VK_SUCCESS) {
		Logger::log(0, "%s: Error - Failed to submit draw command buffer.\n", __FUNCTION__);
		return false;
	}


	// Inform vulkan to copy drawn swapchain image to the surface.
	VkPresentInfoKHR presentInfo{};

	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	// Will be signaled once all commands in the command buffer have been executed (pure GPU internal whereas fence is GPU-CPU synchronization)
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &mRenderData.rdRenderSemaphore;

	// Presentation of swapchain image waits with semaphore until draw commands are finished and final image in framebuffer is completed
	// To let which image to present
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &mRenderData.rdVkbSwapchain.swapchain;
	presentInfo.pImageIndices = &imageIndex;

	// Queue image presentaion  
	result = vkQueuePresentKHR(mRenderData.rdPresentQueue, &presentInfo);

	// Check if vulkan surface no longer matches the swapchain image properties and make sure to recreate the swapchain if it has for optimal performance
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		return recreateSwapchain(mRenderData);
	}
	else 
	{

		if (result != VK_SUCCESS) 
		{
			Logger::log(0, "%s: Error - Failed to present swapchain image. \n", __FUNCTION__);
			return false;
		}
	}



}

bool recreateSwapchain(VkRenderData renderData) {
	return true;
}
