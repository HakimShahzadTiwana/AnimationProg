#pragma once

#include <vector>
#include <string>
#include <memory>
// OpenGL Mathematics Lib
#include <glm/glm.hpp>
// Matrix roation
#include <glm/gtc/matrix_transform.hpp>
// Perpective view
#include <glm/ext/matrix_clip_space.hpp>
//Quaternions
#include <glm/gtx/quaternion.hpp>


// Loader generator to make openGL functions human readable and available
#include <glad/glad.h>

// Lib for window operations (make sure after glad lib since it detects and changes based on glad)
#include <GLFW/glfw3.h>

#include "buffers/frameBuffer/FrameBuffer.h"
#include "buffers/vertexBuffer/VertexBuffer.h"
#include "buffers/uniformBuffer/UniformBuffer.h"
#include "buffers/shaderStorageBuffer/ShaderStorageBuffer.h"
#include "textures/Texture.h"
#include "shaders/Shader.h"
#include "../userInterface/UserInterface.h"
#include "../timer/Timer.h"
#include "../camera/Camera.h"
#include "../models/Model.h"
#include "../models/arrow/ArrowModel.h"
#include "../models/arrow/CoordArrowsModel.h"
#include "../models/spline/SplineModel.h"
#include "../models/gltf/GltfModel.h"

#include "OGLRenderData.h"
#include "../../models/gltf/GltfInstance.h"


class OGLRenderer {

public:

	OGLRenderer(GLFWwindow* window);

	// Initialize and create the OpenGL objects we need for drawing
	// @param width - Width of Renderer
	// @param height - Height of Renderer
	bool init(unsigned int width, unsigned int height);

	// Changes dimentsions fo Renderer
	// @param width - Width of Renderer
	// @param height - Height of Renderer
	void setSize(unsigned int width, unsigned int height);

	// Cleanup of openGl objects 
	void cleanup();

	// Store the triangle and texture data from the model
	// @param vertexData - The model extract the data from
	void uploadData(OGLMesh vertexData);

	// Draws triangles to frame buffer
	void draw();

	// Handles keyboard events
	void handleKeyEvents(int key, int scancode, int action, int mods);
	void handleMovementKeys();


	// Handles mouse events
	void handleMouseButtonEvents(int button, int action, int mods);
	void handleMousePositionEvents(double xPos, double yPos);



private:

	OGLRenderData mRenderData{};

	Shader mBasicShader{};
	Shader mChangedShader{};
	Shader mLineShader{};
	Shader mGltfShader{};
	Shader mGltfGPUShader{};
	Shader mGltfGPUDualQuatShader{};

	FrameBuffer mFrameBuffer{};
	VertexBuffer mVertexBuffer{};
	UniformBuffer mUniformBuffer{};
	ShaderStorageBuffer mGltfShaderStorageBuffer{};
	ShaderStorageBuffer mGltfDualQuatSSBuffer{};

	Texture mTex{};

	Timer mFrameTimer{};
	Timer mMatrixGenerateTimer{};
	Timer mUploadToVBOTimer{};
	Timer mUploadToUBOTimer{};
	Timer mUIGenerateTimer{};
	Timer mUIDrawTimer{};
	Timer mIKTimer{};
	Camera mCamera{};

	UserInterface mUserInterface{};

	CoordArrowsModel mCoordArrowsModel{};
	OGLMesh mCoordArrowsMesh{};
	OGLMesh mIKCoordArrowsMesh{};

	ArrowModel mArrowModel{};
	OGLMesh mStartPosArrowMesh{};
	OGLMesh mEndPosArrowMesh{};
	OGLMesh mQuatPosArrowMesh{};

	SplineModel mSplineModel{};
	OGLMesh mSplineMesh{};

	std::vector<std::shared_ptr<GltfInstance>> mGltfInstances{};
	std::vector<std::shared_ptr<GltfInstance>>	mGltfMatrixInstances{};
	std::vector<std::shared_ptr<GltfInstance>>  mGltfDQInstances{};
	std::vector<std::shared_ptr<GltfModel>> mGltfModels {};

	std::vector<glm::mat4> mModelJointMatrices{};
	std::vector<glm::mat2x4> mModelJointDualQuats{};

	std::unique_ptr<Model> mModel = nullptr;
	std::unique_ptr<OGLMesh> mModelMesh = nullptr;
	std::unique_ptr<OGLMesh> mAllMeshes = nullptr;
	std::shared_ptr<OGLMesh> mLineMesh = nullptr;


		unsigned int mLineIndexCount = 0;
		unsigned int mSkeletonLineIndexCount = 0;
		unsigned int mCoordArrowsLineIndexCount = 0;

		glm::quat mQuatModelOrientation[2] = { glm::quat() , glm::quat() };
		glm::quat mQuatModelOrientationConjugate[2] = { glm::quat() , glm::quat() };
		glm::quat mQuatMix = glm::quat();
		glm::quat mQuatMixConjugate = glm::quat();


		glm::mat4 mViewMatrix = glm::mat4(1.0f);
		glm::mat4 mProjectionMatrix = glm::mat4(1.0f);

	
		bool mMouseLock = false;
		int mMouseXPos = 0;
		int mMouseYPos = 0;
		
		double mLastTickTime = 0.0;

		
		
};
