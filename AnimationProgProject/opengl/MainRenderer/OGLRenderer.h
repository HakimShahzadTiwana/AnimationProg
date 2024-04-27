#pragma once

#include <vector>
#include <string>

// OpenGL Mathematics Lib
#include <glm/glm.hpp>
// Matrix roation
#include <glm/gtc/matrix_transform.hpp>
// Perpective view
#include <glm/ext/matrix_clip_space.hpp>


// Loader generator to make openGL functions human readable and available
#include <glad/glad.h>

// Lib for window operations (make sure after glad lib since it detects and changes based on glad)
#include <GLFW/glfw3.h>

#include "./buffers/frameBuffer/FrameBuffer.h"
#include "./buffers/vertexBuffer/VertexBuffer.h"
#include "./buffers/uniformBuffer/UniformBuffer.h"
#include "./textures/Texture.h"
#include "./shaders/Shader.h"

#include "OGLRenderData.h"


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

		// Hand;es keyboard events
		void handleKeyEvents(int key, int scancode, int action, int mods);

	private:
		GLFWwindow* mWindow = nullptr;

		Shader mBasicShader{};
		Shader mChangedShader{};
		FrameBuffer mFrameBuffer{};
		VertexBuffer mVertexBuffer{};
		UniformBuffer mUniformBuffer{};
		Texture mTex{};

		glm::mat4 mViewMatrix = glm::mat4(1.0f);
		glm::mat4 mProjectionMatrix = glm::mat4(1.0f);


		// Counter of triangles we upload to the renderer
		int mTriangleCount = 0;

		// If render is using the changed shader file 
		bool mUseChangedShader = false;

		int mWidth;
		int mHeight;
		
};
