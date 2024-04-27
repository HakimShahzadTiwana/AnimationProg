#include "VertexBuffer.h"
#include "../Logger/Logger.h"

void VertexBuffer::init() {

	Logger::log(1, "%s: Initing Vertex Buffer...\n", __FUNCTION__);

	// Creates a vertex array
	glGenVertexArrays(1, &mVAO);

	// Creates a vertex object
	glGenBuffers(1, &mVertexVBO);

	// Bind array and buffer
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO);

	// Configures the buffer object - Pointers to the positions and uv properties in the OGLVertex struct
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OGLVertex), (void*)offsetof(OGLVertex, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OGLVertex), (void*)offsetof(OGLVertex, color));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,sizeof(OGLVertex), (void*)offsetof(OGLVertex, uv));

	// Enables the buffer we just configured
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Unbind array and buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	Logger::log(1, "%s: Completed initing buffer.\n", __FUNCTION__);
}

void VertexBuffer::cleanup() {

	Logger::log(1, "%s: Cleaning Vertex Buffer...\n", __FUNCTION__);

	glDeleteBuffers(1, &mVertexVBO);
	glDeleteVertexArrays(1, &mVAO);

	Logger::log(1, "%s: Completed cleaning Vertex Buffer...\n", __FUNCTION__);
}

void VertexBuffer::uploadData(OGLMesh vertexData) {

	Logger::log(1, "%s: Uploading vertex data to Vertex Buffer...\n", __FUNCTION__);

	// Bind vertex buffer and array
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO);

	// Uploads the vertex data to openGL buffer 
	glBufferData(GL_ARRAY_BUFFER, vertexData.vertices.size() * sizeof(OGLVertex), &vertexData.vertices.at(0), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

void VertexBuffer::bind() {

	Logger::log(1, "%s: Binding vertex array.\n", __FUNCTION__);
	glBindVertexArray(mVAO);
}

void VertexBuffer::unbind() {

	Logger::log(1, "%s: Unbinding vertex array.\n", __FUNCTION__);
	glBindVertexArray(0);
}

void VertexBuffer::draw(GLuint mode, unsigned int start,unsigned int num) {

	Logger::log(1, "%s: Drawing...\n", __FUNCTION__);

	glDrawArrays(mode, start, num);

	Logger::log(1, "%s: Completed drawing.\n", __FUNCTION__);

}