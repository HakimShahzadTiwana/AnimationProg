#include "GltfModel.h"
#include "../logger/Logger.h"

void GltfModel::createVertexBuffers() 
{
	// Get reference to the primitives data structure of models mesh
	// in this case we are getting the first primitive structure from the first mesh in the model.
	const tinygltf::Primitive &primitives = mModel->meshes.at(0).primitives.at(0);

	// resize vector according to the size of the attributes vector stores in the file
	mVertexVBO.resize(primitives.attributes.size());

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

int GltfModel::getTriangleCount()
{
	const tinygltf::Primitive& primitives =	mModel->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAccessor = mModel->accessors.at(primitives.indices);
	return indexAccessor.count;
}

bool  GltfModel::loadModel(OGLRenderData& renderData, std::string modelFilename, std::string textureFilename)
{
	// Load texture from file
	if (!mTex.loadTexture(textureFilename,false))
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
		Logger::log(0, "%s error: could not load file '%s'\n",	__FUNCTION__, modelFilename.c_str());
		return false;
	}

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	createVertexBuffers();
	createIndexBuffer();
	glBindVertexArray(0);

	renderData.rdGltfTriangleCount = getTriangleCount();
	return true;
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

