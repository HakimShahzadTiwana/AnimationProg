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


	/* Camera Data*/
	// FOV
	int rdFieldOfView = 90;
	// Yaw
	float rdViewAzimuth = 320.0f;
	// Pitch 
	float rdViewElevation = -15.0f;
	// Movement
	int rdMoveForward = 0;
	int rdMoveRight = 0;
	int rdMoveUp = 0;
	
	// Position
	glm::vec3 rdCameraWorldPosition = glm::vec3(0);


	// Difference between tewo rendered images
	float rdTickDiff = 0.0f;
};