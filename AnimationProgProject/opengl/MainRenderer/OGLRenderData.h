#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glfw/glfw3.h>
#include <string>

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

enum class skinningMode {
	linear = 0,
	dualQuat
};

enum class replayDirection {
	forward = 0,
	backward
};

enum class blendMode {
	fadeinout = 0,
	crossfade,
	additive
};

enum class ikMode {
	off = 0,
	ccd,
	fabrik
};

struct OGLRenderData {

	// GFLW Window instance
	GLFWwindow* rdWindow = nullptr;

	// Dimensions of window
	 int rdWidth = 0;
	 int rdHeight = 0;

	// Number of triangles drawn
	unsigned int rdTriangleCount = 0;

	// Tirangle count for GLTF model
	unsigned int rdGltfTriangleCount = 0;

	// Time for a single frame to be created and rendered
	float rdFrameTime = 0.0f;
	float rdMatrixGenerateTime = 0.0f;
	float rdUploadToVBOTime = 0.0f;
	float rdUploadToUBOTime = 0.0f;
	float rdUIGenerateTime = 0.0f;
	float rdUIDrawTime = 0.0f;
	float rdIKTime = 0.0f;


	// Is the program currently using the second shader
	bool rdUseChangedShader = false;


	/* Camera Data*/
	// FOV
	int rdFieldOfView = 90;
	// Yaw
	float rdViewAzimuth = 0.0f;
	// Pitch 
	float rdViewElevation = -15.0f;
	// Movemen
	int rdMoveForward = 0;
	int rdMoveRight = 0;
	int rdMoveUp = 0;
	
	// Position
	glm::vec3 rdCameraWorldPosition = glm::vec3(0.0f,3.0f,5.0f);


	// Difference between tewo rendered images
	float rdTickDiff = 0.0f;

	/* Spline and Slerp */
	bool rdDrawWorldCoordArrows = true;
	bool rdDrawModelCoordArrows = true;
	bool rdDrawSplineLines = true;
	bool rdResetAnglesAndInterp = true;

	std::vector<int> rdRotXAngle = { 0, 0 };
	std::vector<int> rdRotYAngle = { 0, 0 };
	std::vector<int> rdRotZAngle = { 0, 0 };

	glm::vec3 rdSplineStartVertex = glm::vec3(0.0f);
	glm::vec3 rdSplineStartTangent = glm::vec3(0.0f);
	glm::vec3 rdSplineEndVertex = glm::vec3(0.0f);
	glm::vec3 rdSplineEndTangent = glm::vec3(0.0f);

	float rdInterpValue = 0.0f;

	int rdNumberOfInstances = 0;
	int rdCurrentSelectedInstance = 0;

};