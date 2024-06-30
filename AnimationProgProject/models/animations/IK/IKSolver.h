#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../models/gltf/GltfNode.h"

class IKSolver {
public :
	IKSolver();
	IKSolver(unsigned int interations);

	void setNodes(std::vector<std::shared_ptr<GltfNode>> nodes);
	std::shared_ptr<GltfNode> getIkChainRootNode();

	void setNumIterations(unsigned int iterations);

	bool solveCCD(glm::vec3 target);
	bool solveFABRIK(glm::vec3 target);

private:
	std::vector<std::shared_ptr<GltfNode>> mNodes{};
	unsigned int mIterations = 0;
	float mThreshold = 0.00001f;
	
	std::vector<float> mBoneLengths{};
	std::vector<glm::vec3> mFABRIKNodePositions{};

	void solveFABRIKForward(glm::vec3 target);
	void solveFABRIKBackward(glm::vec3 base);
	void calculateBoneLengths();
	void adjustFABRIKNodes();




};