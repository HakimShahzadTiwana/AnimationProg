#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "./mainRenderer/OGLRenderer.h"

class Model {
public:

	// Initializes model
	void init();

	// Gets model vertex data
	OGLMesh getVertexData();

private:

	// Stores Model Vertex Data
	OGLMesh mVertexData{};
};