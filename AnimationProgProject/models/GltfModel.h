#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include "./textures/Texture.h"
#include "./mainRenderer/OGLRenderData.h"

class GltfModel {
public:

	bool loadModel(OGLRenderData& renderData, std::string modelFilename, std::string textureFilename);

	void draw();

	void cleanup();

	void uploadVertexBuffers();
	void uploadIndexBuffer();

private:

	// pointer to loaded model
	std::shared_ptr<tinygltf::Model> mModel = nullptr;

	// save generated vertex array object
	GLuint mVAO = 0;
	// save vertex buffer object for vertex data
	std::vector<GLuint> mVertexVBO{};
	// save index buffer object
	GLuint mIndexVBO = 0;

	// relation between attrubute type pf glTF model's primitive field and vertex attribute position. (Hardcoded, proper way is to do a dynamic look up of input variables in the shader)
	std::map<std::string, GLint> attributes = { {"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2} };

	Texture mTex{};


	void createVertexBuffers();
	void createIndexBuffer();

	int getTriangleCount();

};


