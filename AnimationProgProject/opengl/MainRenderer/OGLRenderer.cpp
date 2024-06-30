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

	size_t uniformMatrixBufferSize = 2 * sizeof(glm::mat4);
	mUniformBuffer.init(uniformMatrixBufferSize);
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
	if (!mGltfGPUShader.loadShaders("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf_gpu.vert", "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf_gpu.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__, "shader/gltf_gpu.vert", "shader/gltf_gpu.frag");
		return false;
	}

	if (!mGltfGPUDualQuatShader.loadShaders("D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf_gpu_dquat.vert", "D:\\Github_Repos\\AnimationProg\\AnimationProgProject\\Shaders\\gltf_gpu_dquat.frag")) {
		Logger::log(0, "%s: Error - Could not load shaders. \"%s\" and  \"%s\".\n", __FUNCTION__, "shader/gltf_gpu_dquat.vert", "shader/gltf_gpu_dquat.frag");
		return false;
	}

	mUserInterface.init(mRenderData);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3.0);

	mModel = std::make_unique<Model>();

	mModelMesh = std::make_unique<OGLMesh>();
	Logger::log(1, "%s: model mesh storage initialized\n", __FUNCTION__);

	mSkeletonMesh = std::make_shared<OGLMesh>();
	Logger::log(1, "%s: skeleton mesh storage initialized\n", __FUNCTION__);


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

	size_t modelJointMatrixBufferSize = mGltfModel->getJointMatrixSize() * sizeof(glm::mat4);
	mGltfShaderStorageBuffer.init(modelJointMatrixBufferSize);
	Logger::log(1, "%s: glTF joint matrix uniform buffer (size %i bytes) successfully created\n", __FUNCTION__, modelJointMatrixBufferSize);

	size_t modelJointDualQuatBufferSize = mGltfModel->getJointDualQuatsSize() *sizeof(glm::mat2x4);
	mGltfDualQuatSSBuffer.init(modelJointDualQuatBufferSize);
	Logger::log(1, "%s: glTF joint dual quaternions shader storage buffer (size %i bytes) successfully created\n", __FUNCTION__, modelJointDualQuatBufferSize);

	mRenderData.rdSkelSplitNode = mRenderData.rdModelNodeCount - 1;

	mRenderData.rdIkEffectorNode = 19;
	mRenderData.rdIkRootNode = 26;
	mGltfModel->setInverseKinematicsNodes(mRenderData.rdIkEffectorNode,mRenderData.rdIkRootNode);
	mGltfModel->setNumIKIterations(mRenderData.rdIkIterations);

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
	Logger::log(2, "%s: Vertex data uploaded successfully.\n", __FUNCTION__);
}

void OGLRenderer::draw() {

	Logger::log(2, "%s: Drawing...\n", __FUNCTION__);



	// Get Tick time
	double tickTime = glfwGetTime();
	mRenderData.rdTickDiff = tickTime - lastTickTime;

	mRenderData.rdFrameTime = mFrameTimer.stop();
	mFrameTimer.start();
	
	handleMovementKeys();

	mAllMeshes->vertices.clear();


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
	mProjectionMatrix = glm::perspective(glm::radians(static_cast<float>(mRenderData.rdFieldOfView)), static_cast<float>(mRenderData.rdWidth) / static_cast<float>(mRenderData.rdHeight), 0.1f, 100.f);
	mViewMatrix = mCamera.getViewMatrix(mRenderData);

	
	static blendMode lastBlendMode = mRenderData.rdBlendingMode;
	if (lastBlendMode != mRenderData.rdBlendingMode)
	{
		lastBlendMode = mRenderData.rdBlendingMode;
		if (mRenderData.rdBlendingMode != blendMode::additive)
		{
			mRenderData.rdSkelSplitNode = mRenderData.rdModelNodeCount - 1;
		}
		mGltfModel->resetNodeData();
	}

	static int skelSplitNode = mRenderData.rdSkelSplitNode;
	if (skelSplitNode != mRenderData.rdSkelSplitNode)
	{
		mGltfModel->setSkeletonSplitNode(mRenderData.rdSkelSplitNode);
		skelSplitNode = mRenderData.rdSkelSplitNode;
		mGltfModel->resetNodeData();
	}

	static ikMode lastIkMode = mRenderData.rdIkMode;
	if (lastIkMode != mRenderData.rdIkMode)
	{
		mGltfModel->resetNodeData();
		lastIkMode = mRenderData.rdIkMode;
		
		if (mRenderData.rdIkMode == ikMode::off)
		{
			mRenderData.rdIKTime = 0.0f;
		}
	}

	static int numIKIterations = mRenderData.rdIkIterations;
	if (numIKIterations != mRenderData.rdIkIterations) 
	{
		mGltfModel->setNumIKIterations(mRenderData.rdIkIterations);
		mGltfModel->resetNodeData();
		numIKIterations = mRenderData.rdIkIterations;
	}

	static int ikEffectorNode = mRenderData.rdIkEffectorNode;
	static int ikRootNode = mRenderData.rdIkRootNode;
	if (ikEffectorNode != mRenderData.rdIkEffectorNode || ikRootNode != mRenderData.rdIkRootNode)
	{
		mGltfModel->setInverseKinematicsNodes(mRenderData.rdIkEffectorNode,mRenderData.rdIkRootNode);
		mGltfModel->resetNodeData();
		ikEffectorNode = mRenderData.rdIkEffectorNode;
		ikRootNode = mRenderData.rdIkRootNode;
	}

	if (mRenderData.rdPlayAnimation) 
	{
		if (mRenderData.rdBlendingMode == blendMode::crossfade || mRenderData.rdBlendingMode == blendMode::additive) 
		{
			mGltfModel->playAnimation(mRenderData.rdAnimClip,mRenderData.rdCrossBlendDestAnimClip, mRenderData.rdAnimSpeed,mRenderData.rdAnimCrossBlendFactor,mRenderData.rdAnimationPlayDirection);
		}
		else
		{
			mGltfModel->playAnimation(mRenderData.rdAnimClip, mRenderData.rdAnimSpeed,mRenderData.rdAnimBlendFactor,mRenderData.rdAnimationPlayDirection);
		}
	}
	else
	{
		mRenderData.rdAnimEndTime = mGltfModel->getAnimationEndTime(mRenderData.rdAnimClip);
		if (mRenderData.rdBlendingMode == blendMode::crossfade || mRenderData.rdBlendingMode == blendMode::additive) 
		{
			mGltfModel->crossBlendAnimationFrame(mRenderData.rdAnimClip,mRenderData.rdCrossBlendDestAnimClip, mRenderData.rdAnimTimePosition, mRenderData.rdAnimCrossBlendFactor);
		}
		else 
		{
			mGltfModel->blendAnimationFrame(mRenderData.rdAnimClip, mRenderData.rdAnimTimePosition, mRenderData.rdAnimBlendFactor);
		}
	}

	if (mRenderData.rdIkMode != ikMode::off) 
	{
		mIKTimer.start();

		switch (mRenderData.rdIkMode)
		{
		case ikMode::ccd:
			mGltfModel->solveIKByCCD(mRenderData.rdIkTargetPos);
			break;
		case ikMode::fabrik:
			mGltfModel->solveIKByFABRIK(mRenderData.rdIkTargetPos);
		default:
			break;
		}
		
		mRenderData.rdIKTime = mIKTimer.stop();
	}


	if (mRenderData.rdDrawSkeleton)
	{
		mSkeletonMesh = mGltfModel->getSkeleton(true);
	}

	mRenderData.rdMatrixGenerateTime = mMatrixGenerateTimer.stop();

	mUploadToUBOTimer.start();
	std::vector<glm::mat4> matrixData;
	matrixData.push_back(mViewMatrix);
	matrixData.push_back(mProjectionMatrix);
	mUniformBuffer.uploadUboData(matrixData, 0);

	if (mRenderData.rdGPUDualQuatVertexSkinning == skinningMode::dualQuat)
	{
		mGltfDualQuatSSBuffer.uploadSsboData(mGltfModel->getJointDualQuats(), 2);
	}
	else
	{
		mGltfShaderStorageBuffer.uploadSsboData(mGltfModel->getJointMatrices(), 1);
	}


	mRenderData.rdUploadToUBOTime = mUploadToUBOTimer.stop();

	

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

	/* upload vertex data */
	mUploadToVBOTimer.start();

	mRenderData.rdUploadToVBOTime = mUploadToVBOTimer.stop();
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

	if (mRenderData.rdDrawSkeleton)
	{
		mAllMeshes->vertices.insert(mAllMeshes->vertices.end(), mSkeletonMesh->vertices.begin(), mSkeletonMesh->vertices.end());
	}

	/* draw coordiante arrows on target position */
	mIKCoordArrowsLineIndexCount = 0;
	if (mRenderData.rdIkMode != ikMode::off) 
	{
		mIKCoordArrowsMesh = mCoordArrowsModel.getVertexData();
		mIKCoordArrowsLineIndexCount = mIKCoordArrowsMesh.vertices.size();
		std::for_each(mIKCoordArrowsMesh.vertices.begin(), mIKCoordArrowsMesh.vertices.end(),[=](auto& n) 
			{
				n.color /= 2.0f;
				n.position += mRenderData.rdIkTargetPos;
			});

		mAllMeshes->vertices.insert(mAllMeshes->vertices.end(),mIKCoordArrowsMesh.vertices.begin(), mIKCoordArrowsMesh.vertices.end());
	}

	uploadData(*mAllMeshes);

	if (mModelUploadRequired) 
	{
		mGltfModel->uploadVertexBuffers();
		mModelUploadRequired = false;
	}

	mRenderData.rdUploadToVBOTime = mUploadToVBOTimer.stop();

	mLineIndexCount = mStartPosArrowMesh.vertices.size() + mEndPosArrowMesh.vertices.size() + mQuatPosArrowMesh.vertices.size() + mCoordArrowsMesh.vertices.size() + mSplineMesh.vertices.size();

	if (mRenderData.rdDrawSkeleton) 
	{
		mSkeletonLineIndexCount = mSkeletonMesh->vertices.size();
	}
	else 
	{
		mSkeletonLineIndexCount = 0;
	}


	
	// Draw GLTF Model
	if (mRenderData.rdDrawGltfModel)
	{
		if (mRenderData.rdGPUDualQuatVertexSkinning == skinningMode::dualQuat) 
		{
			mGltfGPUDualQuatShader.use();
		}
		else
		{
			mGltfGPUShader.use();
		}
		mGltfModel->draw();
	}

	if (mLineIndexCount > 0)
	{
		mLineShader.use();
		mVertexBuffer.bindAndDraw(GL_LINES, 0, mLineIndexCount);
	}

	/* draw the skeleton last, disable depth test to overlay */
	if (mSkeletonLineIndexCount > 0 && mRenderData.rdDrawSkeleton) 
	{
		glDisable(GL_DEPTH_TEST);
		mLineShader.use();
		mVertexBuffer.bindAndDraw(GL_LINES, mLineIndexCount, mSkeletonLineIndexCount);
		glEnable(GL_DEPTH_TEST);
	}

	if (mIKCoordArrowsLineIndexCount > 0 && mRenderData.rdIkMode != ikMode::off) 
	{
		mLineShader.use();
		mVertexBuffer.bindAndDraw(GL_LINES, mSkeletonLineIndexCount, mIKCoordArrowsLineIndexCount);
	}

	mFrameBuffer.unbindDrawing();


	// Draw content of the frame buffer to the screen
	mFrameBuffer.drawToScreen();


	mUIGenerateTimer.start();
	mUserInterface.createFrame(mRenderData);
	mRenderData.rdUIGenerateTime = mUIGenerateTimer.stop();

	mUIDrawTimer.start();
	mUserInterface.render();
	mRenderData.rdUIDrawTime = mUIDrawTimer.stop();

	// Save last tick time
	lastTickTime = tickTime;

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
	mBasicShader.use();
	mChangedShader.use();
	mLineShader.use();
	mGltfShader.use();
	mGltfGPUShader.use();
	mGltfGPUDualQuatShader.use();

	// Cleanup Texture
	mTex.cleanup();

	// Cleanup VertexBuffer
	mVertexBuffer.cleanup();
	// Cleanup UnfiformBuffer
	mUniformBuffer.cleanup();
	mGltfShaderStorageBuffer.cleanup();
	mGltfDualQuatSSBuffer.cleanup();
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