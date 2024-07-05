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

	std::srand(static_cast<int>(time(NULL)));

	// Init OpenGL via Glad 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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

	// Init vertex buffer
	mVertexBuffer.init();

	size_t uniformMatrixBufferSize = 2 * sizeof(glm::mat4);
	mUniformBuffer.init(uniformMatrixBufferSize);
	Logger::log(1, "%s: uniform buffer successfully created\n", __FUNCTION__);


	if (!mLineShader.loadShaders("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\line.vert", "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\line.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__, "shader/line.vert", "shader/line.frag");
		return false;
	}

	if (!mGltfGPUShader.loadShaders("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf_gpu.vert", "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf_gpu.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__, "shader/gltf_gpu.vert", "shader/gltf_gpu.frag");
		return false;
	}

	if (!mGltfGPUShader.getUniformLocation("aModelStride"))
	{
		return false;
	}

	if (!mGltfGPUDualQuatShader.loadShaders("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf_gpu_dquat.vert", "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf_gpu_dquat.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__, "shader/gltf_gpu_dquat.vert", "shader/gltf_gpu_dquat.frag");
		return false;
	}

	if (!mGltfGPUDualQuatShader.getUniformLocation("aModelStride"))
	{
		return false;
	}


	mUserInterface.init(mRenderData);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3.0);
	mGltfModels.resize(3);
	mGltfModels.at(0) = std::make_shared<GltfModel>();
	std::string modelFilename = "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\assets\\Woman.gltf";
	std::string modelTexFilename = "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Textures\\Woman.png";
	if (!mGltfModels.at(0)->loadModel(mRenderData, modelFilename, modelTexFilename)) {
		Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
		return false;
	}
	mGltfModels.at(0)->uploadVertexBuffers();
	mGltfModels.at(0)->uploadIndexBuffer();

	mGltfModels.at(1) = std::make_shared<GltfModel>();
	modelTexFilename = "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Textures\\Woman2.png";
	if (!mGltfModels.at(1)->loadModel(mRenderData, modelFilename, modelTexFilename)) {
		Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
		return false;
	}
	mGltfModels.at(1)->uploadVertexBuffers();
	mGltfModels.at(1)->uploadIndexBuffer();

	mGltfModels.at(2) = std::make_shared<GltfModel>();
	modelFilename = "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\assets\\dq.gltf";
	modelTexFilename = "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Textures\\dq.png";
	if (!mGltfModels.at(2)->loadModel(mRenderData, modelFilename, modelTexFilename)) {
		Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
		return false;
	}
	mGltfModels.at(2)->uploadVertexBuffers();
	mGltfModels.at(2)->uploadIndexBuffer();

	Logger::log(1, "%s: glTF model '%s' succesfully loaded\n", __FUNCTION__, modelFilename.c_str());


	int numTriangles = 0;	
	for (int i = 0; i < 50; i++)
	{
		int xPos = std::rand() % 40 - 20;
		int zPos = std::rand() % 40 - 20;
		int modelNo = std::rand() % 2;
		mGltfInstances.emplace_back(std::make_shared<GltfInstance>(mGltfModels.at(modelNo), glm::vec2(static_cast<float>(xPos), static_cast<float>(zPos)), true));
		numTriangles += mGltfModels.at(modelNo)->getTriangleCount();
	}

	for (int i = 0; i < 25; ++i) {
		int xPos = std::rand() % 50 - 25;
		int zPos = std::rand() % 20 - 50;
		mGltfInstances.emplace_back(std::make_shared<GltfInstance>(mGltfModels.at(2), glm::vec2(static_cast<float>(xPos),static_cast<float>(zPos)), true));
		numTriangles += mGltfModels.at(2)->getTriangleCount();
	}

	mRenderData.rdTriangleCount = numTriangles;
	mRenderData.rdNumberOfInstances = mGltfInstances.size();

	size_t modelJointMatrixBufferSize = 0;
	size_t modelJointDualQuatBufferSize = 0;
	int jointMatrixSize = 0;
	int jointQuatSize = 0;


	for (const auto& instance : mGltfInstances) {
		jointMatrixSize += instance->getJointMatrixSize();
		modelJointMatrixBufferSize += instance->getJointMatrixSize() * sizeof(glm::mat4);

		jointQuatSize += instance->getJointDualQuatsSize();
		modelJointDualQuatBufferSize += instance->getJointDualQuatsSize() * sizeof(glm::mat2x4);
	}

	mGltfShaderStorageBuffer.init(modelJointMatrixBufferSize);
	Logger::log(1, "%s: glTF joint matrix uniform buffer (size %i bytes) successfully created\n", __FUNCTION__, modelJointMatrixBufferSize);

	mGltfDualQuatSSBuffer.init(modelJointDualQuatBufferSize);
	Logger::log(1, "%s: glTF joint dual quaternions shader storage buffer (size %i bytes) successfully created\n", __FUNCTION__, modelJointDualQuatBufferSize);

	mLineMesh = std::make_shared<OGLMesh>();
	Logger::log(1, "%s: line mesh storage initialized\n", __FUNCTION__);

	mFrameTimer.start();

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

	Logger::log(1, "%s: Set Render size to width : %i and height: %i.\n", __FUNCTION__, width, height);
}

void OGLRenderer::uploadData(OGLMesh vertexData) {

	//mRenderData.rdTriangleCount = vertexData.vertices.size();
	mVertexBuffer.uploadData(vertexData);
	Logger::log(2, "%s: Vertex data uploaded successfully.\n", __FUNCTION__);
}

void OGLRenderer::draw() {

	Logger::log(2, "%s: Drawing...\n", __FUNCTION__);



	// Get Tick time
	double tickTime = glfwGetTime();
	mRenderData.rdTickDiff = tickTime - mLastTickTime;

	mRenderData.rdFrameTime = mFrameTimer.stop();
	mFrameTimer.start();

	handleMovementKeys();


	// Bind frame buffer object which will let it receive the vertex data
	mFrameBuffer.bindDrawing();

	// Setup //
	// Clear screen with a low grey color
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClearDepth(1.0f);
	// Display the set screen color and clear the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enables back face culling which means to draw only the front side of the triangle since we don't really see the back side
	// Hence the back side of the objects will never be seen and the triangles facing "away" don't need to be drawn
	//glEnable(GL_CULL_FACE);

	mMatrixGenerateTimer.start();
	// Projection Matrix for view of world
	// PARAMS - FOV, Aspect Ratio, Near Z distance, Far Z Distance
	mProjectionMatrix = glm::perspective(glm::radians(static_cast<float>(mRenderData.rdFieldOfView)), static_cast<float>(mRenderData.rdWidth) / static_cast<float>(mRenderData.rdHeight), 0.01f, 500.f);
	mViewMatrix = mCamera.getViewMatrix(mRenderData);


	mRenderData.rdIKTime = 0.0f;
	for (auto& instance : mGltfInstances)
	{
		instance->updateAnimation();

		mIKTimer.start();
		instance->solveIK();
		mRenderData.rdIKTime += mIKTimer.stop();
	}

	int selectedInstance = mRenderData.rdCurrentSelectedInstance;
	glm::vec2 modelWorldPos = mGltfInstances.at(selectedInstance)->getWorldPosition();
	glm::quat modelWorldRot = mGltfInstances.at(selectedInstance)->getWorldRotation();


	mLineMesh->vertices.clear();

	/* get gltTF skeleton */
	mSkeletonLineIndexCount = 0;
	for (const auto& instance : mGltfInstances)
	{
		ModelSettings settings = instance->getInstanceSettings();
		if (settings.msDrawSkeleton)
		{
			std::shared_ptr<OGLMesh> mesh = instance->getSkeleton();
			mSkeletonLineIndexCount += mesh->vertices.size();
			mLineMesh->vertices.insert(mLineMesh->vertices.begin(), mesh->vertices.begin(), mesh->vertices.end());
		}
	}

	mCoordArrowsLineIndexCount = 0;
	ModelSettings ikSettings = mGltfInstances.at(selectedInstance)->getInstanceSettings();
	if (ikSettings.msIkMode == ikMode::ccd || ikSettings.msIkMode == ikMode::fabrik)
	{
		mCoordArrowsMesh = mCoordArrowsModel.getVertexData();
		mCoordArrowsLineIndexCount += mCoordArrowsMesh.vertices.size();
		std::for_each(mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end(), [=](auto& n)
			{
				n.color /= 2.0f;
				n.position = modelWorldRot * n.position;
				n.position += ikSettings.msIkTargetWorldPos;
			});

		mLineMesh->vertices.insert(mLineMesh->vertices.end(), mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end());
	}

	mCoordArrowsMesh = mCoordArrowsModel.getVertexData();
	mCoordArrowsLineIndexCount += mCoordArrowsMesh.vertices.size();
	std::for_each(mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end(), [=](auto& n)
		{
			n.color /= 2.0f;
			n.position = modelWorldRot * n.position;
			n.position += glm::vec3(modelWorldPos.x, 0.0f, modelWorldPos.y);
		});

	mLineMesh->vertices.insert(mLineMesh->vertices.end(), mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end());

	mRenderData.rdMatrixGenerateTime = mMatrixGenerateTimer.stop();


	mUploadToUBOTimer.start();
	std::vector<glm::mat4> matrixData;
	matrixData.push_back(mViewMatrix);
	matrixData.push_back(mProjectionMatrix);
	mUniformBuffer.uploadUboData(matrixData, 0);

	mModelJointMatrices.clear();
	mModelJointDualQuats.clear();

	mGltfMatrixInstances.clear();
	mGltfDQInstances.clear();

	unsigned int numTriangles = 0;

	for (const auto& instance : mGltfInstances) 
	{
		ModelSettings settings = instance->getInstanceSettings();
		if (!settings.msDrawModel) 
		{
			continue;
		}

		if (settings.msVertexSkinningMode == skinningMode::dualQuat)
		{
			std::vector<glm::mat2x4> quats = instance->getJointDualQuats();
			mModelJointDualQuats.insert(mModelJointDualQuats.end(), quats.begin(), quats.end());
			mGltfDQInstances.emplace_back(instance);
		}
		else
		{
			std::vector<glm::mat4> mats = instance->getJointMatrices();
			mModelJointMatrices.insert(mModelJointMatrices.end(), mats.begin(), mats.end());
			mGltfMatrixInstances.emplace_back(instance);
		}
		numTriangles += instance->getModel()->getTriangleCount();
	}
	mRenderData.rdTriangleCount = numTriangles;

	mGltfShaderStorageBuffer.uploadSsboData(mModelJointMatrices, 1);
	mGltfDualQuatSSBuffer.uploadSsboData(mModelJointDualQuats, 2);
	mRenderData.rdUploadToUBOTime = mUploadToUBOTimer.stop();


	/* upload vertex data */
	mUploadToVBOTimer.start();

	uploadData(*mLineMesh);

	mRenderData.rdUploadToVBOTime = mUploadToVBOTimer.stop();


	/* draw the glTF models */
	unsigned int jointMatrixSize = mGltfInstances.at(0)->getJointMatrixSize();
	unsigned int matrixPos = 0;

	mGltfGPUShader.use();
	for (const auto& instance : mGltfMatrixInstances) 
	{
		/* set position inside the SSBO */
		mGltfGPUShader.setUniformValue(matrixPos);
		instance->getModel()->draw();
		matrixPos += instance->getJointMatrixSize();
	}

	unsigned int dqPos = 0;

	mGltfGPUDualQuatShader.use();
	for (const auto& instance : mGltfDQInstances)
	{
		mGltfGPUDualQuatShader.setUniformValue(dqPos);
		instance->getModel()->draw();
		dqPos += instance->getJointDualQuatsSize();
	}


	if (mCoordArrowsLineIndexCount > 0) {
		mLineShader.use();
		mVertexBuffer.bindAndDraw(GL_LINES, mSkeletonLineIndexCount, mCoordArrowsLineIndexCount);
	}


	if (mSkeletonLineIndexCount > 0) {
		glDisable(GL_DEPTH_TEST);
		mLineShader.use();
		mVertexBuffer.bindAndDraw(GL_LINES, 0, mSkeletonLineIndexCount);
		glEnable(GL_DEPTH_TEST);
	}

	mFrameBuffer.unbindDrawing();

	mFrameBuffer.drawToScreen();

	mUIGenerateTimer.start();

	ModelSettings settings = mGltfInstances.at(selectedInstance)->getInstanceSettings();
	mUserInterface.createFrame(mRenderData, settings);
	mGltfInstances.at(selectedInstance)->setInstanceSettings(settings);
	mGltfInstances.at(selectedInstance)->checkForUpdates();

	mRenderData.rdUIGenerateTime = mUIGenerateTimer.stop();

	mUIDrawTimer.start();
	mUserInterface.render();
	mRenderData.rdUIDrawTime = mUIDrawTimer.stop();

	mLastTickTime = tickTime;

	Logger::log(2, "%s: Time taken to execute draw function %f\n", __FUNCTION__, mRenderData.rdFrameTime);

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
	if (io.WantCaptureMouse) {
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

	for (int i = 0; i < mGltfModels.size(); ++i) 
	{
		mGltfModels.at(i)->cleanup();
		mGltfModels.at(i).reset();
	}

	mGltfGPUDualQuatShader.cleanup();
	mGltfGPUShader.cleanup();
	mUserInterface.cleanup();
	mLineShader.cleanup();
	mVertexBuffer.cleanup();
	mGltfShaderStorageBuffer.cleanup();
	mGltfDualQuatSSBuffer.cleanup();
	mUniformBuffer.cleanup();
	mFrameBuffer.cleanup();

	Logger::log(1, "%s: Renderer cleaned successfully.\n", __FUNCTION__);
}