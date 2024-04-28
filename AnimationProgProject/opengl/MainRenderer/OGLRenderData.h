#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glfw/glfw3.h>

struct OGLVertex {
	// Postion Co-ordinates
	glm::vec3 position;

	// Color Data
	glm::vec3 color;

	// Texture Co-ordinates
	glm::vec2 uv;

};

struct OGLMesh {
	// Vector of Vertex data
	std::vector<OGLVertex> vertices;
};

struct OGLRenderData {
	GLFWwindow* rdWindow = nullptr;
	unsigned int rdWidth = 0;
	unsigned int rdHeight = 0;
	unsigned int rdTriangleCount = 0;
	float rdFrameTime = 0.0f;
	float rdUIGenerateTime = 0.0f;
	bool rdUseChangedShader = false;
	int rdFieldOfView = 90;
};