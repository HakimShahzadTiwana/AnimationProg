#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

glm::mat4 Camera::getViewMatrix(OGLRenderData& renderData)
{
	// Get pitch nad yaw radian values
	float azimRad = glm::radians(renderData.rdViewAzimuth);
	float elevRad = glm::radians(renderData.rdViewElevation);

	// Get sin cos values of pitch and yaw
	float sinAzim = glm::sin(azimRad);
	float cosAzim = glm::cos(azimRad);
	float sinElev = glm::sin(elevRad);
	float cosElev = glm::cos(elevRad);



	mViewDirection = glm::normalize(glm::vec3(sinAzim * cosElev, sinElev, -cosAzim * cosElev));

	mRightDirection = glm::normalize(glm::cross(mViewDirection, mWorldUpVector));
	mUpDirection = glm::normalize(glm::cross(mRightDirection, mViewDirection));

	renderData.rdCameraWorldPosition += (renderData.rdMoveForward * renderData.rdTickDiff * mViewDirection) + (renderData.rdMoveRight * renderData.rdTickDiff * mRightDirection) + (renderData.rdMoveUp * renderData.rdTickDiff * mUpDirection);

	return glm::lookAt(renderData.rdCameraWorldPosition, renderData.rdCameraWorldPosition + mViewDirection, mWorldUpVector);
}

