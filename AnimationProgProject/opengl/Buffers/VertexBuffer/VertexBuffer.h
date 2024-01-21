#pragma once 

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../MainRenderer/OGLRenderData.h"

class VertexBuffer {
	public:

		// Sets up buffers
		void init();

		// Copies data into buffer
		// @param vertexData - Data to copy to buffer
		void uploadData(OGLMesh vertexData);

		// Enables drawing to vertex buffer (For when we want to use multiple buffers and avoid any unexpected results)
		void bind();

		// Disables drawing to vertex buffer (For when we want to use multiple buffers and avoid any unexpected results)
		void unbind();

		// Draws the currently bound vertex array (by sending it to the gpu)
		// @param mode - Rendering mode (GL_TRIANGLES for drawing triangles, other modes to draw lines etc.)
		// @param start - The starting index of the array
		// @param num - Number of elements to render
		void draw(GLuint mode, unsigned int start, unsigned int num);

		// Cleans up vertex buffer
		void cleanup();
	
	private:

		// Handle to store the vertex array (contains the vertex buffer)
		GLuint mVAO = 0;
		 
		// Handle to store the vertex buffer (contains vertex and texture data)
		GLuint mVertexVBO = 0;

};
