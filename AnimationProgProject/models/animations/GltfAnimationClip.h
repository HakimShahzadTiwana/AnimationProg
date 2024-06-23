#pragma once
#include <string>
#include <vector>
#include <memory>
#include <tiny_gltf.h>
#include "../gltf/GltfNode.h"
#include "GltfAnimationChannel.h"


class GltfAnimationClip {
public:
	GltfAnimationClip(std::string name);


	// Store the loaded channels in a vector and forward the parameters to the new channel object
	void addChannel(std::shared_ptr<tinygltf::Model> model, tinygltf::Animation anim, tinygltf::AnimationChannel channel);

	// Update the model nodes with data from a specific time point
	void setAnimationFrame(std::vector<std::shared_ptr<GltfNode>> nodes, std::vector<bool> additiveMask, float time);

	void blendAnimationFrame(std::vector<std::shared_ptr<GltfNode>> nodes, std::vector<bool> additiveMask, float time,float blendFactor);

	float getClipEndTime();

	std::string getClipName();

private:
	std::vector<std::shared_ptr<GltfAnimationChannel>> mAnimationChannels;
	std::string mClipName;

};