#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include <glm/gtx/dual_quaternion.hpp>
#include "textures/Texture.h"
#include "mainRenderer/OGLRenderData.h"
#include "GltfNode.h"

#include "../animations/GltfAnimationClip.h"


class GltfModel {
public:

	bool loadModel(OGLRenderData& renderData, std::string modelFilename, std::string textureFilename);



	void draw();

	void cleanup();

	void uploadVertexBuffers();
	void uploadIndexBuffer();
	void applyCPUVertexSkinning();
	int getJointMatrixSize();
	int getJointDualQuatsSize();
	std::vector<glm::mat4> getJointMatrices();
	std::vector<glm::mat2x4> getJointDualQuats();
	std::shared_ptr<OGLMesh> getSkeleton(bool enableSkinning);
	void setSkeletonSplitNode(int nodeNum);
	std::string getNodeName(int nodeNum);


	// Bind pose and animation blending
	void playAnimation(int animNum, float speedDivider,float blendFactor);
	void blendAnimationFrame(int animNumber, float time,float blendFactor);

	// Cross fade blending
	void playAnimation(int sourceAnimNum, int destAnimNum,float speedDivider, float blendFactor);
	void crossBlendAnimationFrame(int sourceAnimNumber,int destAnimNumber, float time, float blendFactor);

	void resetNodeData();
	float getAnimationEndTime(int animNum);
	std::string getClipName(int animNum);

private:

	// pointer to loaded model
	std::shared_ptr<tinygltf::Model> mModel = nullptr;
	std::shared_ptr<GltfNode> mRootNode = nullptr;
	std::shared_ptr<OGLMesh> mSkeletonMesh = nullptr;

	// save generated vertex array object
	GLuint mVAO = 0;
	// save vertex buffer object for vertex data
	std::vector<GLuint> mVertexVBO{};
	// save index buffer object
	GLuint mIndexVBO = 0;

	// relation between attrubute type pf glTF model's primitive field and vertex attribute position. (Hardcoded, proper way is to do a dynamic look up of input variables in the shader)
	std::map<std::string, GLint> attributes = { {"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2}, {"JOINTS_0" , 3}, {"WEIGHTS_0", 4}};

	Texture mTex{};

	// For storing bind pose data
	std::vector<glm::tvec4<uint16_t>> mJointVec{};
	std::vector<glm::vec4> mWeightVec{};
	std::vector<glm::mat4> mInverseBindMatrices{};
	std::vector<glm::mat4> mJointMatrices{};
	std::vector<int> mNodeToJoint{};
	std::vector<int> mAttribAccessors{};
	std::vector<glm::vec3> mAlteredPositions{};
	std::vector<glm::mat2x4> mJointDualQuats{};
	std::vector<std::shared_ptr<GltfNode>> mNodeList;
	std::vector<bool> mAdditiveAnimationMask{};
	std::vector<bool> mInvertedAdditiveAnimationMask{};

	// Animation
	std::vector<GltfAnimationClip> mAnimClips{};

	void createVertexBuffers();
	void createIndexBuffer();

	int getTriangleCount();

	void getInverseBindMatrices();
	void getJointData();
	void getWeightData();

	void getNodes(std::shared_ptr<GltfNode> treeNode);
	void getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix);
	void updateNodeMatrices(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix);
	void updateJointMatricesAndQuats(std::shared_ptr<GltfNode> treeNode);
	void getSkeletonPerNode(std::shared_ptr<GltfNode> treeNode, bool enableSkinning);

	void getAnimations();
	void resetNodeData(std::shared_ptr<GltfNode> treeNode,glm::mat4 parentNodeMatrix);
	void updateAdditiveMask(std::shared_ptr<GltfNode> treeNode, int splitNodeNum);

};


