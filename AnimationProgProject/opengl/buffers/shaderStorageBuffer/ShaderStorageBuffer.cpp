#include <glm/gtc/type_ptr.hpp>
#include "ShaderStorageBuffer.h"
#include "../logger/Logger.h"
void ShaderStorageBuffer::init(size_t bufferSize)
{
	mBufferSize = bufferSize;
	// Generate Uniform Buffer
	glGenBuffers(1, &mSsboBuffer);

	// Bind uniform buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboBuffer);

	// Allocate mem for 2 4x4 matrices 
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void ShaderStorageBuffer::uploadSsboData(std::vector<glm::mat4> bufferData, int bindingPoint)
{
	if (bufferData.size() == 0)
	{
		return;
	}

	size_t buffersize = bufferData.size() * sizeof(glm::mat4);
	// Bind buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboBuffer);

	// Upload data
	// PARAMS: BufferType, upload offset in buffer, size of upload, values)
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffersize, bufferData.data());

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPoint, mSsboBuffer, 0, buffersize);

	// Unbind buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void ShaderStorageBuffer::uploadSsboData(std::vector<glm::mat2x4> bufferData, int bindingPoint) 
{
	if (bufferData.size() == 0)
	{
		return;
	}

	size_t buffersize = bufferData.size() * sizeof(glm::mat2x4);
	// Bind buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboBuffer);

	// Upload data
	// PARAMS: BufferType, upload offset in buffer, size of upload, values)
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffersize, bufferData.data());

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPoint, mSsboBuffer, 0, buffersize);

	// Unbind buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::cleanup()
{
	glDeleteBuffers(1, &mSsboBuffer);
}
