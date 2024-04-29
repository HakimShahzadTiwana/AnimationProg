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

	// GFLW Window instance
	GLFWwindow* rdWindow = nullptr;

	// Dimensions of window
	unsigned int rdWidth = 0;
	unsigned int rdHeight = 0;

	// Number of triangles drawn
	unsigned int rdTriangleCount = 0;

	// Time for a single frame to be created and rendered
	float rdFrameTime = 0.0f;

	// Time for a single frame to be created
	float rdUIGenerateTime = 0.0f;

	// Is the program currently using the second shader
	bool rdUseChangedShader = false;

	// FOV
	int rdFieldOfView = 90;

	// Camera Yaw
	float rdViewAzimuth = 320.0f;

	// Camera Pitch 
	float rdViewElevation = -15.0f;
};