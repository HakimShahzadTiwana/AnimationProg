#include <glm/gtc/type_ptr.hpp>
#include "UniformBuffer.h"
#include "../logger/Logger.h"
void UniformBuffer::init(size_t bufferSize)
{
	mBufferSize = bufferSize;
	// Generate Uniform Buffer
	glGenBuffers(1, &mUboBuffer);

	// Bind uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);

	// Allocate mem for 2 4x4 matrices 
	glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);


	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void UniformBuffer::uploadUboData(std::vector<glm::mat4> bufferData, int bindingPoint)
{
	if (bufferData.size() == 0) 
	{
		return;
	}

	size_t buffersize = bufferData.size() * sizeof(glm::mat4);
	// Bind buffer
	glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);

	// Upload data
	// PARAMS: BufferType, upload offset in buffer, size of upload, values)
	glBufferSubData(GL_UNIFORM_BUFFER, 0, buffersize, bufferData.data());

	glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, mUboBuffer, 0, buffersize);

	// Unbind buffer
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void UniformBuffer::cleanup()
{
	glDeleteBuffers(1, &mUboBuffer);
}
