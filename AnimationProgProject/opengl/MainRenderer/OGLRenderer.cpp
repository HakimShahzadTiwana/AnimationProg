#include "OGLRenderer.h"
#include <algorithm>
#include <glm/gtx/spline.hpp>
#include "../Logger/Logger.h"
#include <imgui_impl_glfw.h>


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


	if (!mLineShader.loadShaders("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\line.vert", "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\line.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__, "shader/line.vert", "shader/line.frag");
		return false;
	}
	if (!mGltfShader.loadShaders("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf.vert", "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__, "shader/gltf.vert", "shader/gltf.frag");
		return false;
	}

	mUserInterface.init(mRenderData);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3.0);

	mModel = std::make_unique<Model>();

	mModelMesh = std::make_unique<OGLMesh>();
	Logger::log(1, "%s: model mesh storage initialized\n", __FUNCTION__);

	mAllMeshes = std::make_unique<OGLMesh>();
	Logger::log(1, "%s: global mesh storage initialized\n", __FUNCTION__);

	mGltfModel = std::make_shared<GltfModel>();
	std::string modelFilename = "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\assets\\Woman.gltf";
	std::string modelTexFilename = "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Textures\\Woman.png";
	Logger::log(1, "%s: Gltf model mesh storage initialized\n", __FUNCTION__);
	
	if (!mGltfModel->loadModel(mRenderData, modelFilename,modelTexFilename)) {
		Logger::log(0, "%s:Error - Could not load Gltf model from file.\n", __FUNCTION__);
		return false;
	}
	mGltfModel->uploadIndexBuffer();
	Logger::log(1, "%s: Gltf model loaded\n", __FUNCTION__);

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
	
	//mRenderData.rdTriangleCount = vertexData.vertices.size();
	mVertexBuffer.uploadData(vertexData);
	Logger::log(1, "%s: Vertex data uploaded successfully.\n", __FUNCTION__);
}

void OGLRenderer::draw() {

	Logger::log(1, "%s: Drawing...\n", __FUNCTION__);



	// Get Tick time
	double tickTime = glfwGetTime();
	mRenderData.rdTickDiff = tickTime - lastTickTime;

	// Get Start time
	static float prevFrameStartTime = 0.0;
	float frameStartTime = glfwGetTime();

	handleMovementKeys();

	mAllMeshes->vertices.clear();

	// Setup //

	// Bind frame buffer object which will let it receive the vertex data
	mFrameBuffer.bindDrawing();

	// Clear screen with a low grey color
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClearDepth(1.0f);
	// Display the set screen color and clear the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enables back face culling which means to draw only the front side of the triangle since we don't really see the back side
	// Hence the back side of the objects will never be seen and the triangles facing "away" don't need to be drawn
	//glEnable(GL_CULL_FACE);

	// Draw triangles stored in the buffer //

	// Projection Matrix for view of world
	// PARAMS - FOV, Aspect Ratio, Near Z distance, Far Z Distance
	mProjectionMatrix = glm::perspective(glm::radians(static_cast<float>(mRenderData.rdFieldOfView)),static_cast<float>(mRenderData.rdWidth) / static_cast<float>(mRenderData.rdHeight),0.1f, 100.f);

	// Get time
	float time = glfwGetTime();

	glm::mat4 view = glm::mat4(1.0);

	// Load Shader program to enable processing or vertex data
	if (mRenderData.rdUseChangedShader)
	{
		//mChangedShader.use();
		// Creates rotation matrix around the z axis by an amount of "time" radians
		//view = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else
	{
		//mBasicShader.use();
		//view = glm::rotate(glm::mat4(1.0f), -time, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	// Combine the the rotation camera position with the view matrix
	mViewMatrix = mCamera.getViewMatrix(mRenderData);// *view;

	mUniformBuffer.uploadUboData(mViewMatrix, mProjectionMatrix);

	// Reset Angle and interp Values to Zero when UI button pressed
	if (mRenderData.rdResetAnglesAndInterp)
	{
		mRenderData.rdResetAnglesAndInterp = false;

		mRenderData.rdRotXAngle = { 0,0 };
		mRenderData.rdRotYAngle = { 0,0 };
		mRenderData.rdRotZAngle = { 0,0 };

		mRenderData.rdInterpValue = 0;

		mRenderData.rdSplineStartVertex = glm::vec3(-4, 1, -2);
		mRenderData.rdSplineStartTangent = glm::vec3(-10, -8, 8);
		mRenderData.rdSplineEndVertex = glm::vec3(4, 2, -2);
		mRenderData.rdSplineEndTangent = glm::vec3(-6, 5, -6);


		mRenderData.rdDrawModelCoordArrows = true;
		mRenderData.rdDrawWorldCoordArrows = true;
		mRenderData.rdDrawSplineLines = true;
	}

	// Create quarternion from angles

	for (int i = 0; i < 2; i++)
	{
		// Using the angles to create a rotation vector in radians which is converted to a normalized quarternion for orientation of the model

		mQuatModelOrientation[i] = glm::normalize(glm::quat(glm::vec3(
			glm::radians(static_cast<float>(mRenderData.rdRotXAngle[i])),
			glm::radians(static_cast<float>(mRenderData.rdRotYAngle[i])),
			glm::radians(static_cast<float>(mRenderData.rdRotZAngle[i]))
		)));

		mQuatModelOrientationConjugate[i] = glm::conjugate(mQuatModelOrientation[i]);
	}

	// Interpolate between the two quarternions
	mQuatMix = glm::slerp(mQuatModelOrientation[0], mQuatModelOrientation[1], mRenderData.rdInterpValue);
	mQuatMixConjugate = glm::conjugate(mQuatMix);

	// position the cube on th current spline position
	glm::vec3 interpolatedPosition = glm::hermite(mRenderData.rdSplineStartVertex,mRenderData.rdSplineStartTangent,mRenderData.rdSplineEndVertex,mRenderData.rdSplineEndTangent,mRenderData.rdInterpValue);

	// draw static coordinate system
	mCoordArrowsMesh.vertices.clear();
	if (mRenderData.rdDrawWorldCoordArrows)
	{
		mCoordArrowsMesh = mCoordArrowsModel.getVertexData();
		std::for_each(mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end(), [=](auto& n)
		{
			n.color /= 2.0f;
		});
		mAllMeshes->vertices.insert(mAllMeshes->vertices.end(),mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end());
	}

	mStartPosArrowMesh.vertices.clear();
	mEndPosArrowMesh.vertices.clear();
	mQuatPosArrowMesh.vertices.clear();

	if (mRenderData.rdDrawModelCoordArrows)
	{
		// Arrow Start Position
		mStartPosArrowMesh = mArrowModel.getVertexData();
		std::for_each(mStartPosArrowMesh.vertices.begin(), mStartPosArrowMesh.vertices.end(), [=](auto& n)
		{
				glm::quat position = glm::quat(0.0f, n.position.x, n.position.y, n.position.z);
				glm::quat newPosition = mQuatModelOrientation[0] * position * mQuatModelOrientationConjugate[0];
				n.position.x = newPosition.x;
				n.position.y = newPosition.y;
				n.position.z = newPosition.z;
				n.position += n.position + mRenderData.rdSplineStartVertex;
				n.color = glm::vec3(0.0f, 0.8f, 0.8f);
		});
		mAllMeshes->vertices.insert(mAllMeshes->vertices.end(),mStartPosArrowMesh.vertices.begin(), mStartPosArrowMesh.vertices.end());

		// Arrow End Position
		mEndPosArrowMesh = mArrowModel.getVertexData();
		std::for_each(mEndPosArrowMesh.vertices.begin(), mEndPosArrowMesh.vertices.end(), [=](auto& n)
		{
			glm::quat position = glm::quat(0.0f, n.position.x, n.position.y, n.position.z);
			glm::quat newPosition = mQuatModelOrientation[0] * position * mQuatModelOrientationConjugate[0];
			n.position.x = newPosition.x;
			n.position.y = newPosition.y;
			n.position.z = newPosition.z;
			n.position += n.position + mRenderData.rdSplineEndVertex;
			n.color = glm::vec3(0.0f, 0.8f, 0.8f);
		});
		mAllMeshes->vertices.insert(mAllMeshes->vertices.end(), mEndPosArrowMesh.vertices.begin(), mEndPosArrowMesh.vertices.end());

		// Arrow to show quarternion orientation changes
		mQuatPosArrowMesh = mArrowModel.getVertexData();
		std::for_each(mQuatPosArrowMesh.vertices.begin(), mQuatPosArrowMesh.vertices.end(),[=](auto& n)
		{
			glm::quat position = glm::quat(0.0f, n.position.x, n.position.y, n.position.z);
			glm::quat newPosition = mQuatMix * position * mQuatMixConjugate;
			n.position.x = newPosition.x;
			n.position.y = newPosition.y;
			n.position.z = newPosition.z;
			n.position += n.position + interpolatedPosition;

		});
		mAllMeshes->vertices.insert(mAllMeshes->vertices.end(),mQuatPosArrowMesh.vertices.begin(), mQuatPosArrowMesh.vertices.end());
	}

	// Draw Spline
	mSplineMesh.vertices.clear();
	if (mRenderData.rdDrawSplineLines)
	{
		mSplineMesh = mSplineModel.createVertexData(25,mRenderData.rdSplineStartVertex, mRenderData.rdSplineStartTangent, mRenderData.rdSplineEndVertex, mRenderData.rdSplineEndTangent);
		mAllMeshes->vertices.insert(mAllMeshes->vertices.end(),mSplineMesh.vertices.begin(), mSplineMesh.vertices.end());
	}

	// draw model
	*mModelMesh = mModel->getVertexData();
	mRenderData.rdTriangleCount = mModelMesh->vertices.size();
	std::for_each(mModelMesh->vertices.begin(), mModelMesh->vertices.end(), [=](auto& n)
	{
		glm::quat position = glm::quat(0.0f, n.position.x, n.position.y, n.position.z);
		glm::quat newPosition = mQuatMix * position * mQuatMixConjugate;
		n.position.x = newPosition.x;
		n.position.y = newPosition.y;
		n.position.z = newPosition.z;
		n.position += n.position + interpolatedPosition;

	});
	mAllMeshes->vertices.insert(mAllMeshes->vertices.end(),mModelMesh->vertices.begin(), mModelMesh->vertices.end());

	mGltfModel->uploadVertexBuffers();
	uploadData(*mAllMeshes);

	mLineIndexCount = mStartPosArrowMesh.vertices.size() + mEndPosArrowMesh.vertices.size() + mQuatPosArrowMesh.vertices.size() + mCoordArrowsMesh.vertices.size() + mSplineMesh.vertices.size();

	// Draw lines first
	if (mLineIndexCount > 0)
	{
		mLineShader.use();

		mVertexBuffer.bind();
		mVertexBuffer.draw(GL_LINES,0,mLineIndexCount);
		mVertexBuffer.unbind();
	}

	// Draw GLTF Model
	mGltfShader.use();
	mGltfModel->draw();

	// Draw model last
	mBasicShader.use();

	// Bind texture to draw textured triangles
	mTex.bind();

	// Bind vertex buffer so that triangle data is available
	mVertexBuffer.bind();

	// Sends vertex data to gpu to be processed by the shaders
	mVertexBuffer.draw(GL_TRIANGLES, mLineIndexCount, mRenderData.rdTriangleCount);

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

	// Save last tick time
	lastTickTime = tickTime;

	Logger::log(1, "%s: Time taken to execute draw function %f\n", __FUNCTION__, mRenderData.rdFrameTime);
	
}

void OGLRenderer::handleKeyEvents(int key, int scancode, int action, int mods)
{
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//mRenderData.rdUseChangedShader = !mRenderData.rdUseChangedShader;
		//Logger::log(1, "%s: Space pressed... Toggling useChangeShader to %d\n", __FUNCTION__, mRenderData.rdUseChangedShader);
	}

}

void OGLRenderer::handleMovementKeys()
{
	// Forward Movement
	mRenderData.rdMoveForward = 0;
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_W) == GLFW_PRESS) 
	{
		mRenderData.rdMoveForward += 1;
	}

	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_S) == GLFW_PRESS) 
	{
		mRenderData.rdMoveForward -= 1;
	}

	// Right movement
	mRenderData.rdMoveRight = 0;
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_A) == GLFW_PRESS) 
	{
		mRenderData.rdMoveRight -= 1;
	}
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		mRenderData.rdMoveRight += 1;
	}

	// Upward movement
	mRenderData.rdMoveUp = 0;
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		mRenderData.rdMoveUp += 1;
	}
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		mRenderData.rdMoveUp -= 1;
	}
}


void OGLRenderer::handleMouseButtonEvents(int button, int action, int mods)
{
	// Get userinterface input/output object
	ImGuiIO& io = ImGui::GetIO();

	if (button >= 0 && button < ImGuiMouseButton_COUNT) {
		io.AddMouseButtonEvent(button, action == GLFW_PRESS);
	}
	
	// Return if the mouse is needed for UI
	if (io.WantCaptureMouse) {
		return;
	}

	// Check is right mouse button is pressed
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) 
	{
		mMouseLock = !mMouseLock;
	}

	if (mMouseLock) 
	{
		// Hide cursor if we are in locked mode
		glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Set mouse mode to raw motion, disabling the cursor is needed to for this
		if (glfwRawMouseMotionSupported()) 
		{
			// Raw mode omits any extra mouse feature such as acceleration
			glfwSetInputMode(mRenderData.rdWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
	}
	else 
	{
		glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}

void OGLRenderer::handleMousePositionEvents(double xPos, double yPos)
{
	// Get userinterface input/output object
	ImGuiIO& io = ImGui::GetIO();

	io.AddMousePosEvent((float)xPos, (float)yPos);

	// Return if the mouse is needed for UI
	if (io.WantCaptureMouse){
		return;
	}

	// Calculate difference from saved mouse pos and current mouse pos
	int mouseMoveRelX = static_cast<int>(xPos) - mMouseXPos;
	int mouseMoveRelY = static_cast<int>(yPos) - mMouseYPos;

	if (mMouseLock) 
	{
		// X pos for Azimuth (yaw)

		// Scale down by 10 to have more control ( mouse sensistivity)
		mRenderData.rdViewAzimuth += mouseMoveRelX / 10.0f;

		// Make sure ranges are between 0 and 360 
		if (mRenderData.rdViewAzimuth < 0.0) 
		{
			mRenderData.rdViewAzimuth += 360.0;
		}
		if (mRenderData.rdViewAzimuth >= 360.0) 
		{
			mRenderData.rdViewAzimuth -= 360.0;
		}

		// y Pos for elevation (pitch) 

		mRenderData.rdViewElevation -= mouseMoveRelY / 10.0;


		// Range between -89 and 89 to make sure that azimuth values arent turned around 

		if (mRenderData.rdViewElevation > 89.0)
		{
			mRenderData.rdViewElevation = 89.0;
		}
		if (mRenderData.rdViewElevation < -89.0)
		{
			mRenderData.rdViewElevation = -89.0;
		}

		// Save current mouse pos
		mMouseXPos = static_cast<int>(xPos);
		mMouseYPos = static_cast<int>(yPos);
	}
}


void OGLRenderer::cleanup() {

	Logger::log(1, "%s: Cleaning up renderer...\n", __FUNCTION__);
	// Cleanup Shaders
	mBasicShader.cleanup();
	mChangedShader.cleanup();
	mLineShader.cleanup();

	// Cleanup Texture
	mTex.cleanup();

	// Cleanup VertexBuffer
	mVertexBuffer.cleanup();
	// Cleanup UnfiformBuffer
	mUniformBuffer.cleanup();
	// Cleanup FrameBuffer
	mFrameBuffer.cleanup();

	// Cleanup UserInterface
	mUserInterface.cleanup();

	// Cleanup GLTF Model
	mGltfModel->cleanup();
	mGltfModel.reset();
	mGltfShader.cleanup();

	Logger::log(1, "%s: Renderer cleaned successfully.\n", __FUNCTION__);
}