#include "FrameBuffer.h"
#include "../Logger/Logger.h"

bool FrameBuffer::init(unsigned int width, unsigned int height) {
	
	Logger::log(1, "%s: Initing Frame buffer...\n", __FUNCTION__);
	// Store dimension of buffer
	mBufferWidth = width;
	mBufferHeight = height;

	// Create an OpenGl Frame Buffer object
	glGenFramebuffers(1, &mBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mBuffer);

	// Create a texture with the same size of the buffer and bind the texture as a 2D texture type
	glGenTextures(1, &mColorTex);
	glBindTexture(GL_TEXTURE_2D, mColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// Set properties of the texture (since some drivers refuse to display textures if not set) //
	
	// Downscaling/Minification if drawn far away
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// Upscaling/ Magnification if drawn close up
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Clamp to edge if drawing is outside the defined area of the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Unbind texture by binding it to invalid texture id (0) to avoid further modifications 
	glBindTexture(GL_TEXTURE_2D, 0);

	// Bind texture as so called texture attachment 0 (We can bind multiple texture attachments to the buffer)
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mColorTex, 0);

	// If we don't need to show or reuse the result of a drawing operation we can use a render buffer instead of a texture
	// It can be written to, like the texture in the framebuffer, but can not be read easily
	// Useful for intermediate buffers that are valid for a single frame and the content is not needed for more than a single drawing process

	// While a pixel attachment is about to be written, the depth buffer will check wether the pixel is closer to the viewer compared
	// to the pixel already in that position. If it is the depth buffer will be updated, if it is not then the write will be discarded
	glGenRenderbuffers(1, &mDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	// Bind the created render buffer as a depth attachment 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

	// Unbind the render and frame buffer
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (!checkComplete()) {
	Logger::log(0, "%s: Error - Frame buffer init failed.\n", __FUNCTION__);
	return false;
	}

	Logger::log(1, "%s: Frame buffer init successful.\n", __FUNCTION__);
	return true;


}


bool FrameBuffer::resize(unsigned int newWidth, unsigned int newHeight) {

	Logger::log(1, "%s: Resizing frame buffer...\n", __FUNCTION__);

	mBufferWidth = newWidth;
	mBufferHeight = newHeight;

	// Remove created openGL objects from frame buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDeleteTextures(1, &mColorTex);
	glDeleteRenderbuffers(1, &mDepthBuffer);
	glDeleteFramebuffers(1, &mBuffer);

	// Create new frame buffer
	if (!init(newWidth, newHeight)) {
		Logger::log(0, "%s: Error - Could not resize frame buffer.\n", __FUNCTION__);
		return false;
	}

	Logger::log(1, "%s: Frame buffer resized successfully.\n", __FUNCTION__);
	return true;
}

bool FrameBuffer::checkComplete() {


	Logger::log(1, "%s: Checking Frame buffer....\n", __FUNCTION__);

	glBindFramebuffer(GL_FRAMEBUFFER, mBuffer);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (result != GL_FRAMEBUFFER_COMPLETE) {
		Logger::log(0, "%s: Error - Frame buffer check failed!\n", __FUNCTION__);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Logger::log(1, "%s: Frame buffer check passed.\n", __FUNCTION__);
	return true;
}

void FrameBuffer::bindDrawing() {

	Logger::log(1, "%s: Binding frame buffer to draw.\n", __FUNCTION__);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBuffer);
}

void FrameBuffer::unbindDrawing() {

	Logger::log(1, "%s: Unbinding frame buffer from draw.\n", __FUNCTION__);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::cleanup() {

	Logger::log(1, "%s: Cleaning up frame buffer...\n", __FUNCTION__);

	unbindDrawing();

	// Remove created openGL objects from frame buffer
	glDeleteTextures(1, &mColorTex);
	glDeleteRenderbuffers(1, &mDepthBuffer);
	glDeleteFramebuffers(1, &mBuffer);

	Logger::log(1, "%s: Frame buffer was cleaned successfully.\n", __FUNCTION__);

}

void FrameBuffer::drawToScreen() {

	Logger::log(1, "%s: Drawing frame buffer to screen...\n", __FUNCTION__);

	// Bind the frame buffer to for read
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mBuffer);
	// Unbind for draw
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	// Memory Copy (Blit) the contents to the frame buffer to the window
	glBlitFramebuffer(0, 0, mBufferWidth, mBufferHeight, 0, 0, mBufferWidth, mBufferHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	Logger::log(1, "%s: Completed drawing frame buffer to screen...\n", __FUNCTION__);
}

