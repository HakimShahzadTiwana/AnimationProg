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



class GltfModel {
public:

	bool loadModel(OGLRenderData& renderData, std::string modelFilename, std::string textureFilename);

	void draw();

	void cleanup();

	void uploadVertexBuffers();
	void uploadIndexBuffer();
	void applyCPUVertexSkinning(bool enableSkinning);
	int getJointMatrixSize();
	std::vector<glm::mat4> getJointMatrices();
	int getJointDualQuatsSize();
	std::vector<glm::mat2x4> getJointDualQuats();

private:

	// pointer to loaded model
	std::shared_ptr<tinygltf::Model> mModel = nullptr;
	std::shared_ptr<GltfNode> mRootNode = nullptr;
	// save generated vertex array object
	GLuint mVAO = 0;
	// save vertex buffer object for vertex data
	std::vector<GLuint> mVertexVBO{};
	// save index buffer object
	GLuint mIndexVBO = 0;

	// relation between attrubute type pf glTF model's primitive field and vertex attribute position. (Hardcoded, proper way is to do a dynamic look up of input variables in the shader)
	std::map<std::string, GLint> attributes = { {"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2} };

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


	void createVertexBuffers();
	void createIndexBuffer();

	int getTriangleCount();

	void getInverseBindMatrices();
	void getJointData();
	void getWeightData();

	void getNodes(std::shared_ptr<GltfNode> treeNode);
	void getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix);


};


