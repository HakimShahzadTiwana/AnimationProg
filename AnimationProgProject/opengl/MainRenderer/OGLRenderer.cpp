#include "OGLRenderer.h"
#include "../Logger/Logger.h"
bool OGLRenderer::init(unsigned int width, unsigned int height) {

	// Init OpenGL via Glad 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		Logger::log(0, "%s: Error - Could not init openGL via Glad.\n", __FUNCTION__);
		return false;
	}

	// Check glad version
	if (!GLAD_GL_VERSION_4_6) {
		Logger::log(0, "%s: Error - Invalid Glad version.\n", __FUNCTION__);
		return false;
	}

	// Init frame buffer
	if (!mFrameBuffer.init(width, height)) {
		Logger::log(0, "%s: Error - Could not init Frame buffer.\n", __FUNCTION__);
		return false;
	}

	// Load Texture
	if (mTex.loadTexture("textures/crate.png")) {
		Logger::log(0, "%s: Error - Could not load texture \"%s\".\n", __FUNCTION__,"textures/crate.png");
		return false;
	}

	// Init vertex buffer
	mVertexBuffer.init();

	// Load shaders
	if (!mBasicShader.loadShaders("shader/basic.vert", "shader/basic.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__,"shader/basic.vert", "shader/basic.frag");
		return false;
	}

	
	// Init successful
	Logger::log(1, "%s: Renderer Init Successful.\n", __FUNCTION__);
	return true;
}

void OGLRenderer::setSize(unsigned int width, unsigned int height) {

	// Resize buffer
	mFrameBuffer.resize(width, height);
	// Resize viewport
	glViewport(0, 0, width, height);

	Logger::log(1, "%s: Set Render size to width : %i and height: %i.\n", __FUNCTION__,width,height);
}

void OGLRenderer::uploadData(OGLMesh vertexData) {
	
	mTriangleCount = vertexData.vertices.size();
	mVertexBuffer.uploadData(vertexData);
	Logger::log(1, "%s: Vertex data uploaded successfully.\n", __FUNCTION__);
}

void OGLRenderer::draw() {

	Logger::log(1, "%s: Drawing...\n", __FUNCTION__);
	// Setup //

	// Bind frame buffer object which will let it receive the vertex data
	mFrameBuffer.bind();

	// Clear screen with a low grey color 
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Display the set screen color and clear the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enables back face culling which means to draw only the front side of the triangle since we don't really see the back side
	// Hence the back side of the objects will never be seen and the triangles facing "away" don't need to be drawn
	glEnable(GL_CULL_FACE);

	// Draw triangles stored in the buffer //

	// Load Shader program to enable processing or vertex data
	mBasicShader.use();

	// Bind texture to draw textured triangles
	mTex.bind();

	// Bind vertex buffer so that triangle data is available
	mVertexBuffer.bind();

	// Sends vertex data to gpu to be processed by the shaders
	mVertexBuffer.draw(GL_TRIANGLES, 0, mTriangleCount);
		
	// Unbind
	mVertexBuffer.unbind();
	mTex.unbind();
	mFrameBuffer.unbind();

	// Draw content of the frame buffer to the screen
	mFrameBuffer.drawToScreen();

}


void OGLRenderer::cleanup() {

	Logger::log(1, "%s: Cleaning up renderer...\n", __FUNCTION__);
	// Cleanup Shader
	mBasicShader.cleanup();

	// Cleanup Texture
	mTex.cleanup();

	// Cleanup VertexBuffer
	mVertexBuffer.cleanup();

	// Cleanup FrameBuffer
	mFrameBuffer.cleanup();

	Logger::log(1, "%s: Renderer cleaned successfully.\n", __FUNCTION__);
}