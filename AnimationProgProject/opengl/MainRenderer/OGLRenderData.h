#pragma once
#include <glm/glm.hpp>
#include <vector>

struct OGLVertex {
	// Postion Co-ordinates
	glm::vec3 position;

	// Color Data
	glm::vec3 color;

	// Texture Co-ordinates
	glm::vec2 uv;

};

struct OGLMesh {
	// Vector of Vertex data
	std::vector<OGLVertex> vertices;
};