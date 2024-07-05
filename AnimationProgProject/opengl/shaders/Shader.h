#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader {
public:

	// Load shaders from files and generate OpenGL shaders
	bool loadShaders(std::string vertexShaderFileName, std::string fragmentShaderFileName);

	// Instructs graphic card to use the shader for draw operation
	void use();

	bool getUniformLocation(std::string uniformName);
	void setUniformValue(int value);


	// Free created OpenGL shader 
	void cleanup();

private:

	GLuint mShaderProgram = 0;
	GLint mUniformLocation = -1;

	GLuint readShader(std::string shaderFileNme, GLuint shaderType);

};