#include <glm/gtc/type_ptr.hpp>
#include "UniformBuffer.h"
#include "../logger/Logger.h"
void UniformBuffer::init()
{
	// Generate Uniform Buffer
	glGenBuffers(1, &mUboBuffer);

	// Bind uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);

	// Allocate mem for 2 4x4 matrices 
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);


	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void UniformBuffer::uploadUboData(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{

	// Bind buffer
	glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);

	// Upload data
	// PARAMS: BufferType, upload offset in buffer, size of upload, values)
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));

	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) , sizeof(glm::mat4), glm::value_ptr(projectionMatrix));

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, mUboBuffer, 0, 2 * sizeof(glm::mat4));

	// Unbind buffer
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void UniformBuffer::cleanup()
{
	glDeleteBuffers(1, &mUboBuffer);
}
