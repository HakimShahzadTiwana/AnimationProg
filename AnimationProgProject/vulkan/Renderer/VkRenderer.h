#pragma once
#include "../opengl/mainRenderer/OGLRenderData.h";
#include "VkRenderData.h";


class VkRenderer {

public:

	void uploadData(OGLMesh vertexData);

	bool draw();

	bool init(unsigned int height, unsigned int width);

	VkRenderData mRenderData{};

	bool recreateSwapchain(VkRenderData renderData);


	int mTriangleCount = 0;


	VkBuffer mVertexBuffer;

};