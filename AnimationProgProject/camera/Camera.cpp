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

	return glm::lookAt(mWorldPos,mWorldPos + mViewDirection, mWorldUpVector);
}

