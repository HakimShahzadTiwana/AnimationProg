#include "OGLRenderer.h"
#include "../Logger/Logger.h"

OGLRenderer::OGLRenderer(GLFWwindow* window)
{
	mRenderData.rdWindow = window;
	
}

bool OGLRenderer::init(unsigned int width, unsigned int height) {

	mRenderData.rdWidth = width;
	mRenderData.rdHeight = height;

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
	if (!mTex.loadTexture("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Textures\\crate.png")) {
		Logger::log(0, "%s: Error - Could not load texture \"%s\".\n", __FUNCTION__,"textures/crate.png");
		return false;
	}

	// Init vertex buffer
	mVertexBuffer.init();

	// Load shaders
	if (!mBasicShader.loadShaders("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\basic.vert", "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\basic.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__,"shader/basic.vert", "shader/basic.frag");
		return false;
	}

	mUniformBuffer.init();
	Logger::log(1, "%s: uniform buffer successfully created\n", __FUNCTION__);

	if (!mChangedShader.loadShaders("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\changed.vert", "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\changed.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__, "shader/changed.vert", "shader/changed.frag");
		return false;
	}

	mUserInterface.init(mRenderData);
	// Init successful
	Logger::log(1, "%s: Renderer Init Successful.\n", __FUNCTION__);
	return true;
}

void OGLRenderer::setSize(unsigned int width, unsigned int height) {
	
	if (width == 0 || height == 0) {
		return;
	}

	mRenderData.rdWidth = width;
	mRenderData.rdHeight = height;

	// Resize buffer
	mFrameBuffer.resize(width, height);
	// Resize viewport
	glViewport(0, 0, width, height);

	Logger::log(1, "%s: Set Render size to width : %i and height: %i.\n", __FUNCTION__,width,height);
}

void OGLRenderer::uploadData(OGLMesh vertexData) {
	
	mRenderData.rdTriangleCount = vertexData.vertices.size();
	mVertexBuffer.uploadData(vertexData);
	Logger::log(1, "%s: Vertex data uploaded successfully.\n", __FUNCTION__);
}

void OGLRenderer::draw() {

	Logger::log(1, "%s: Drawing...\n", __FUNCTION__);

	// Get Start time 
	static float prevFrameStartTime = 0.0;
	float frameStartTime = glfwGetTime();

	// Setup //

	// Bind frame buffer object which will let it receive the vertex data
	mFrameBuffer.bindDrawing();

	// Clear screen with a low grey color 
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Display the set screen color and clear the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enables back face culling which means to draw only the front side of the triangle since we don't really see the back side
	// Hence the back side of the objects will never be seen and the triangles facing "away" don't need to be drawn
	glEnable(GL_CULL_FACE);

	// Draw triangles stored in the buffer //

	// Set projection view 
	glm::vec3 cameraPosition = glm::vec3(0.4f, 0.3f, 1.0f);
	glm::vec3 cameraLookAtPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

	// Projection Matrix for view of world 
	// PARAMS - FOV, Aspect Ratio, Near Z distance, Far Z Distance 
	mProjectionMatrix = glm::perspective(glm::radians(static_cast<float>(mRenderData.rdFieldOfView)),static_cast<float>(mRenderData.rdWidth) / static_cast<float>(mRenderData.rdHeight),0.1f, 100.f);

	// Get time 
	float time = glfwGetTime();

	glm::mat4 view = glm::mat4(1.0);

	// Load Shader program to enable processing or vertex data
	if (mRenderData.rdUseChangedShader) 
	{
		mChangedShader.use();
		// Creates rotation matrix around the z axis by an amount of "time" radians
		view = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else 
	{
		mBasicShader.use();
		view = glm::rotate(glm::mat4(1.0f), -time, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	// Combine the the rotation camera position
	mViewMatrix = glm::lookAt(cameraPosition, cameraLookAtPosition, cameraUpVector) * view;
	mUniformBuffer.uploadUboData(mViewMatrix, mProjectionMatrix);
	// Bind texture to draw textured triangles
	mTex.bind();

	// Bind vertex buffer so that triangle data is available
	mVertexBuffer.bind();

	// Sends vertex data to gpu to be processed by the shaders
	mVertexBuffer.draw(GL_TRIANGLES, 0, mRenderData.rdTriangleCount);
		
	// Unbind
	mVertexBuffer.unbind();
	mTex.unbind();
	mFrameBuffer.unbindDrawing();

	// Draw content of the frame buffer to the screen
	mFrameBuffer.drawToScreen();

	mUIGenerateTimer.start();
	mUserInterface.createFrame(mRenderData);
	mRenderData.rdUIGenerateTime = mUIGenerateTimer.stop();

	mUserInterface.render();
	
	// Calculate time taken to complete function  
	mRenderData.rdFrameTime = frameStartTime - prevFrameStartTime;
	prevFrameStartTime = frameStartTime;
	Logger::log(1, "%s: Time taken to execute draw function %f\n", __FUNCTION__, mRenderData.rdFrameTime);

}

void OGLRenderer::handleKeyEvents(int key, int scancode, int action, int mods)
{
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
		mRenderData.rdUseChangedShader = !mRenderData.rdUseChangedShader;
		Logger::log(1, "%s: Space pressed... Toggling useChangeShader to %d\n", __FUNCTION__, mRenderData.rdUseChangedShader);
	}

}


void OGLRenderer::cleanup() {

	Logger::log(1, "%s: Cleaning up renderer...\n", __FUNCTION__);
	// Cleanup Shaders
	mBasicShader.cleanup();
	mChangedShader.cleanup();
	// Cleanup Texture
	mTex.cleanup();

	// Cleanup VertexBuffer
	mVertexBuffer.cleanup();
	// Cleanup UnfiformBuffer
	mUniformBuffer.cleanup();
	// Cleanup FrameBuffer
	mFrameBuffer.cleanup();

	//Cleanup UserInterface
	mUserInterface.cleanup();

	Logger::log(1, "%s: Renderer cleaned successfully.\n", __FUNCTION__);
}