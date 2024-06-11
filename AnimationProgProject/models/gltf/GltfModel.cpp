
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

#include "GltfModel.h"
#include "../logger/Logger.h"


bool  GltfModel::loadModel(OGLRenderData& renderData, std::string modelFilename, std::string textureFilename)
{
	// Load texture from file
	if (!mTex.loadTexture(textureFilename, false))
	{
		return false;
	}

	// Create model pointer
	mModel = std::make_shared < tinygltf::Model>();

	tinygltf::TinyGLTF gltfLoader;
	std::string loaderErrors;
	std::string loaderWarnings;
	bool result = false;
	result = gltfLoader.LoadASCIIFromFile(mModel.get(), &loaderErrors, &loaderWarnings, modelFilename);
	if (!loaderWarnings.empty())
	{
		Logger::log(0, "%s: warnings while loading glTF	model:\n % s\n", __FUNCTION__, loaderWarnings.c_str());
	}
	if (!loaderErrors.empty())
	{
		Logger::log(0, "%s: errors while loading glTF model:\n % s\n", __FUNCTION__, loaderErrors.c_str());
	}
	if (!result) {
		Logger::log(0, "%s error: could not load file '%s'\n", __FUNCTION__, modelFilename.c_str());
		return false;
	}

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	createVertexBuffers();
	createIndexBuffer();
	glBindVertexArray(0);

	renderData.rdGltfTriangleCount = getTriangleCount();

	getInverseBindMatrices();
	getJointData();
	getWeightData();
	
	// Get Root data from file and create RootNode
	int rootData = mModel->scenes.at(0).nodes.at(0);
	mRootNode = GltfNode::createRoot(rootData);


	// Set Node values for TRS
	getNodeData(mRootNode, glm::mat4(1.0f));

	// read children of node from glTF file and add empty child node to 
	getNodes(mRootNode);

	mRootNode->printTree();

	return true;
}


void GltfModel::createVertexBuffers() 
{
	// Get reference to the primitives data structure of models mesh
	// in this case we are getting the first primitive structure from the first mesh in the model.
	const tinygltf::Primitive &primitives = mModel->meshes.at(0).primitives.at(0);

	// resize vector according to the size of the attributes vector stores in the file
	mVertexVBO.resize(primitives.attributes.size());

	mAttribAccessors.resize(primitives.attributes.size());

	// Loop over the attributes of the primitive 
	for (const auto& attrib : primitives.attributes) 
	{
		// Get attribute type
		const std::string attribType = attrib.first;
		// Save attribute index number
		const int accessorNum = attrib.second;

		// Get accessor from current attribute
		const tinygltf::Accessor &accessor = mModel->accessors.at(accessorNum);
		// Get bufferview from accessor
		const tinygltf::BufferView &bufferView = mModel->bufferViews[accessor.bufferView];
		// Get buffer from bufferview
		const tinygltf::Buffer& buffer = mModel->buffers[bufferView.buffer];

		// Skip-over/filter-out attribute types that are not needed
		if (attribType.compare("POSITION") != 0 && attribType.compare("NORMAL") != 0 && attribType.compare("TEXCOORD_0") != 0) 
		{
			continue;
		}

		if (attribType.compare("POSITION")) 
		{
			int numPositionEntries = accessor.count;
			mAlteredPositions.resize(numPositionEntries);
			Logger::log(1, "%s: loaded %i vertices from glTF file\n", __FUNCTION__,numPositionEntries);
		}

		mAttribAccessors.at(attributes.at(attribType)) = accessorNum;

		// Save the size of data
		int dataSize = 1;
		switch (accessor.type) 
		{
		case TINYGLTF_TYPE_SCALAR:
			dataSize = 1;
			break;
		case TINYGLTF_TYPE_VEC2:
			dataSize = 2;
			break;
		case TINYGLTF_TYPE_VEC3:
			dataSize = 3;
			break;
		case TINYGLTF_TYPE_VEC4:
			dataSize = 4;
			break;
		default:
			Logger::log(0, "%s error: accessor %i uses data size % i\n", __FUNCTION__, accessorNum,dataSize);
			break;

		}

		// Get the data type
		GLuint dataType = GL_FLOAT;
		switch (accessor.componentType) 
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			dataType = GL_FLOAT;
			break;
		default:
			Logger::log(0, "%s error: accessor %i uses unknown data type % i\n", __FUNCTION__, accessorNum, dataType);
			break;
		}
		
		// Create vertex buffer objects
		glGenBuffers(1, &mVertexVBO[attributes[attribType]]);
		// Bind it to active vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[attributes[attribType]]);


		// Configure the created vertex buffer
		glVertexAttribPointer(attributes[attribType], dataSize, dataType, GL_FALSE, 0, (void*) 0);
		glEnableVertexAttribArray(attributes[attribType]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void GltfModel::createIndexBuffer()
{
	glGenBuffers(1, &mIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
}

void GltfModel::uploadVertexBuffers()
{
	// Example model has vertex pos data in accessor 0, normal data in accessor 1, and texture data in accessor 2
	for (int i = 0; i < 3; i++) 
	{
		const tinygltf::Accessor& accessor = mModel->accessors.at(i);
		const tinygltf::BufferView& bufferView = mModel->bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = mModel->buffers[bufferView.buffer];

		// Copy data from tinygltf loader to gpu
		glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
}

void GltfModel::uploadIndexBuffer() 
{
	// Get the accessor for the index data from the primitives of the mesh and get the buffer
	const tinygltf::Primitive& primitives =mModel->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAccessor =mModel->accessors.at(primitives.indices);
	const tinygltf::BufferView& indexBufferView =mModel->bufferViews[indexAccessor.bufferView];
	const tinygltf::Buffer& indexBuffer = mModel->buffers[indexBufferView.buffer];

	// Copy the corresponding part of the indexBuffer data to the GPU
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferView.byteLength, &indexBuffer.data.at(0) + indexBufferView.byteOffset, GL_STATIC_DRAW);

}

void GltfModel::applyVertexSkinning(bool enableSkinning)
{
	const tinygltf::Accessor& accessor = mModel->accessors.at(mAttribAccessors.at(0));
	const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

	std::memcpy(mAlteredPositions.data(), &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);

	if (enableSkinning) {
		for (int i = 0; i < mJointVec.size(); ++i) {
			glm::ivec4 jointIndex = glm::make_vec4(mJointVec.at(i));
			glm::vec4 weightIndex = glm::make_vec4(mWeightVec.at(i));
			glm::mat4 skinMat = weightIndex.x * mJointMatrices.at(jointIndex.x) + weightIndex.y * mJointMatrices.at(jointIndex.y) + weightIndex.z * mJointMatrices.at(jointIndex.z) + weightIndex.w * mJointMatrices.at(jointIndex.w);
			mAlteredPositions.at(i) = skinMat * glm::vec4(mAlteredPositions.at(i), 1.0f);
		}
	}

}

int GltfModel::getTriangleCount()
{
	const tinygltf::Primitive& primitives =	mModel->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAccessor = mModel->accessors.at(primitives.indices);
	return indexAccessor.count;
}

void GltfModel::getInverseBindMatrices()
{
	// Get accessor index for skin 
	const tinygltf::Skin& skin = mModel->skins.at(0);
	int invBindMatAccessor = skin.inverseBindMatrices;

	const tinygltf::Accessor& accessor = mModel->accessors.at(invBindMatAccessor);
	const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

	mInverseBindMatrices.resize(skin.joints.size());
	mJointMatrices.resize(skin.joints.size());

	std::memcpy(mInverseBindMatrices.data(), &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);
}

void GltfModel::getJointData()
{
	// Get Joint acessor attribute
	std::string jointsAccessorAttrib = "JOINTS_0";
	int jointsAccessor = mModel->meshes.at(0).primitives.at(0).attributes.at(jointsAccessorAttrib);
	Logger::log(1, "%s: using accessor %i to get %s\n", __FUNCTION__, jointsAccessor,jointsAccessorAttrib.c_str());

	// Search for joint buffer through acessor
	const tinygltf::Accessor& accessor = mModel->accessors.at(jointsAccessor);
	const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);


	int jointVecSize = accessor.count;
	Logger::log(1, "%s: %i short vec4 in JOINTS_0\n", __FUNCTION__, jointVecSize);

	// Copy joint data into vector
	mJointVec.resize(jointVecSize);
	std::memcpy(mJointVec.data(), &buffer.data.at(0) + bufferView.byteOffset,bufferView.byteLength);

	// Save Node to Joint mapping. (Joint index saved at Node number location)
	mNodeToJoint.resize(mModel->nodes.size());
	const tinygltf::Skin& skin = mModel->skins.at(0);
	for (int i = 0; i < skin.joints.size(); ++i) {
		int destinationNode = skin.joints.at(i);
		mNodeToJoint.at(destinationNode) = i;
		Logger::log(2, "%s: joint %i affects node %i\n", __FUNCTION__, i, destinationNode);
	}

}
void GltfModel::getWeightData() {
	std::string weightsAccessorAttrib = "WEIGHTS_0";
	int weightAccessor = mModel->meshes.at(0).primitives.at(0).attributes.at(weightsAccessorAttrib);
	Logger::log(1, "%s: using accessor %i to get %s\n", __FUNCTION__, weightAccessor,weightsAccessorAttrib.c_str());

	const tinygltf::Accessor& accessor = mModel->accessors.at(weightAccessor);
	const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

	int weightVecSize = accessor.count;
	Logger::log(1, "%s: %i vec4 in WEIGHTS_0\n", __FUNCTION__, weightVecSize);
	mWeightVec.resize(weightVecSize);

	std::memcpy(mWeightVec.data(), &buffer.data.at(0) + bufferView.byteOffset,
		bufferView.byteLength);
}



void GltfModel::getNodes(std::shared_ptr<GltfNode> treeNode)
{
	int	nodeNum = treeNode->getNodeNum();
	std::vector<int> childNodes = mModel->nodes.at(nodeNum).children;

	// Remove nodes with skin/mesh data because it will confuse the skeleton
	auto removeIt = std::remove_if(childNodes.begin(), childNodes.end(), [&](int num)
		{
			return mModel->nodes.at(num).skin != -1;
		});
	childNodes.erase(removeIt, childNodes.end());

	treeNode->addChilds(childNodes);

	glm::mat4 treeNodeMatrix = treeNode->getNodeMatrix();

	for (auto& childNode : treeNode->getChilds()) 
	{
		getNodeData(childNode, treeNodeMatrix);
		getNodes(childNode);
	}


}
void GltfModel::getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix) 
{
	// Get node number
	int nodeNum = treeNode->getNodeNum();
	// Get node data by its number from the loaded gltf Model 
	const tinygltf::Node& node = mModel->nodes.at(nodeNum);

	// Set name
	treeNode->setNodeName(node.name);

	// Set TRS and get derived data
	if (node.translation.size()) 
	{
		treeNode->setTranslation(glm::make_vec3(node.translation.data()));
	}
	if (node.scale.size())
	{
		treeNode->setScale(glm::make_vec3(node.scale.data()));
	}
	if (node.rotation.size())
	{
		treeNode->setRotation(glm::make_quat(node.rotation.data()));
	}

	treeNode->calculateLocalTRSMatrix();
	treeNode->calculateNodeMatrix(parentNodeMatrix);

	// Store joint transformations
	mJointMatrices.at(mNodeToJoint.at(nodeNum)) = treeNode->getNodeMatrix() * mInverseBindMatrices.at(mNodeToJoint.at(nodeNum));
}



void GltfModel::cleanup()
{
	glDeleteBuffers(mVertexVBO.size(), mVertexVBO.data());
	glDeleteBuffers(1, &mVAO);
	glDeleteBuffers(1, &mIndexVBO);
	mTex.cleanup();
	mModel.reset();
}

void GltfModel::draw() 
{
	const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAccessor = mModel->accessors.at(primitives.indices);

	GLuint drawMode = GL_TRIANGLES;

	switch (primitives.mode) {
	case TINYGLTF_MODE_TRIANGLES:
		drawMode = GL_TRIANGLES;
		break;
	default:
		Logger::log(0, "%s error: unknown draw mode %i\n", __FUNCTION__, drawMode);
		break;
	}

	mTex.bind();
	glBindVertexArray(mVAO);
	glDrawElements(drawMode, indexAccessor.count, indexAccessor.componentType, nullptr);
	glBindVertexArray(0);
	mTex.unbind();

}

