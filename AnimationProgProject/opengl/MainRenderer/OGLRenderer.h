#pragma once

#include <vector>
#include <string>

// OpenGL Mathematics Lib
#include <glm/glm.hpp>

// Loader generator to make openGL functions human readable and available
#include <glad/glad.h>

// Lib for window operations (make sure after glad lib since it detects and changes based on glad)
#include <GLFW/glfw3.h>

#include "Framebuffer.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "Shader.h"

#include "OGLRenderData.h"


class OGLRenderer {

	public:

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

	private:
		
		Shader mBasicShader{};
		FrameBuffer mFrameBuffer{};
		VertexBuffer mVertexBuffer{};
		Texture mTex{};

		// Counter of triangles we upload to the renderer
		int mTriangleCount = 0;

};
