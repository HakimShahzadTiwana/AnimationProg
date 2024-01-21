#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class FrameBuffer {
	public:

		// Init frame buffer
		// @param width - Width of buffer
		// @param height - Height of buffer
		bool init(unsigned int width, unsigned int height);
		
		// Resizes the frame buffer
		// @param newWidth - New width of buffer
		// @param newHeight - New height of buffer
		bool resize(unsigned int newWidth, unsigned int newHeight);

		// Enables drawing to frame buffer (For when we want to use multiple buffers and avoid any unexpected results)
		void bindDrawing();

		// Disables drawing to frame buffer (For when we want to use multiple buffers and avoid any unexpected results)
		void unbindDrawing();

		// Copies data to GLFW window
		void drawToScreen();

		// Cleans up the Frame buffer
		void cleanup();

	private:
		// Store current width of buffer
		unsigned int mBufferWidth = 640;

		// Store current height of buffer
		unsigned int mBufferHeight = 480;

		// (Internal Buffer) Overall frame buffer we need to draw
		GLuint mBuffer = 0;

		// (Internal Buffer) Color texture we use as data storage for frame buffer
		GLuint mColorTex = 0;

		// (Internal Buffer) Stores distance from the viewer for every pixel and ensures only the one nearest will be drawn
		GLuint mDepthBuffer = 0;

		// Checks wither the frme buffer contains all the components required to draw
		bool checkComplete();

};