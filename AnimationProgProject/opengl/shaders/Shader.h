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

	// Free created OpenGL shader 
	void cleanup();

private:

	GLuint mShaderProgram = 0;
	// 
	GLuint readShader(std::string shaderFileNme, GLuint shaderType);

};