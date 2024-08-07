#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


class GltfNode : public std::enable_shared_from_this<GltfNode> {
public:

	// Create Root Node
	static std::shared_ptr<GltfNode> createRoot(int rootNodeNum);

	// Add Child Nodes
	void addChilds(std::vector<int> childNodes);
	// Get Child Nodes
	std::vector<std::shared_ptr<GltfNode>> getChilds();

	std::shared_ptr<GltfNode> getParentNode();

	glm::quat getLocalRotation();
	glm::quat getGlobalRotation();
	glm::vec3 getGlobalPosition();

	void updateNodeAndChildMatrices();

	// Get Current Node number
	int getNodeNum();


	// Setters
	void setNodeName(std::string name);
	void setScale(glm::vec3 scale);
	void setTranslation(glm::vec3 translation);
	void setRotation(glm::quat rotation);
	void setWorldPosition(glm::vec3 worldPos);
	void setWorldRotation(glm::vec3 worldPRot);
	glm::vec3 getWorldPosition();

	// Calculate TRS
	void calculateLocalTRSMatrix();

	// Calculate Node Mat
	void calculateNodeMatrix();
	// Get Node Mat
	glm::mat4 getNodeMatrix();

	// Print Tree structure
	void printTree();
	std::string getNodeName();

	void blendScale(glm::vec3 scale, float blendFactor);
	void blendTranslation(glm::vec3 translation,float blendFactor);
	void blendRotation(glm::quat rotation,float blendFactor);


private:


	int mNodeNum = 0;
	std::string mNodeName;

	std::weak_ptr<GltfNode> mParentNode;

	// Vector of child nodes
	std::vector<std::shared_ptr<GltfNode>> mChildNodes{};
	
	// Tranform Data of node
	glm::vec3 mScale = glm::vec3(1.0f);
	glm::vec3 mTranslation = glm::vec3(0.0f);
	glm::quat mRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	glm::vec3 mWorldPosition = glm::vec3(0.0f);
	glm::vec3 mWorldRotation = glm::vec3(0.0f);

	// Blend Data of node
	glm::vec3 mBlendScale = glm::vec3(1.0f);
	glm::vec3 mBlendTranslation = glm::vec3(0.0f);
	glm::quat mBlendRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	// TRS: Translation * Rotation * Scale;
	glm::mat4 mLocalTRSMatrix = glm::mat4(1.0f);
	//  mNodeMatrix = parentNodeMatrix * mLocalTRSMatrix;
	glm::mat4 mNodeMatrix = glm::mat4(1.0f);

	glm::mat4 mInverseBindMatrix = glm::mat4(1.0f);

	void printNodes(std::shared_ptr<GltfNode> startNode, int indent);

};