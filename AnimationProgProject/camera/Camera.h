#pragma once
#include <glm/glm.hpp>
#include "./../opengl/mainRenderer/OGLRenderData.h"

class Camera {
public:
	glm::mat4 getViewMatrix(OGLRenderData& renderData);

private:
	
	glm::vec3 mViewDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 mRightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 mUpDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 mWorldUpVector = glm::vec3(0.0f, 1.0f, 0.0f);



};	
