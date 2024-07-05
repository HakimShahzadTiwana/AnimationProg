#include <fstream>
#include "../Logger/Logger.h"
#include "Shader.h"

bool Shader::loadShaders(std::string vertexShaderFileName, std::string fragmentShaderFileName) {

	Logger::log(1, " %s : Loading Shaders...\n", __FUNCTION__);

	GLuint vertexShader = readShader(vertexShaderFileName, GL_VERTEX_SHADER);

	if (!vertexShader) {

		Logger::log(0, " %s : Error - Failed to load Vertex Shader.\n", __FUNCTION__);

		return false;
	}

	GLuint fragmentShader = readShader(fragmentShaderFileName, GL_FRAGMENT_SHADER);

	if (!fragmentShader) {
		
		Logger::log(0, " %s : Error - Failed to load Fragment Shader.\n", __FUNCTION__);

		return false;
	}

	// OpenGL calls to create shader objects and link them

	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, vertexShader);
	glAttachShader(mShaderProgram, fragmentShader);
	glLinkProgram(mShaderProgram);

	GLint isProgramLinked;

	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &isProgramLinked);
	if (!isProgramLinked) {
	
		Logger::log(0, "%s : Error - Failed to link Shaders.\n", __FUNCTION__);
		return false;
	}

	// Extracts location of block with passed in name from the compiled shader program
	GLint uboIndex = glGetUniformBlockIndex(mShaderProgram, "Matrices");

	// The location is then bound to passed index of the uniform buffer in the shader file (see basic.vert and changed.vert)
	glUniformBlockBinding(mShaderProgram, uboIndex, 0);


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	Logger::log(1, " %s : Loaded Shaders Successfully.\n", __FUNCTION__);


	return true;

}

void Shader::cleanup() {
	glDeleteProgram(mShaderProgram);
}

GLuint Shader::readShader(std::string shaderFileName, GLuint shaderType) {

	Logger::log(1, "%s : Reading shader file %s ... \n", __FUNCTION__, shaderFileName.c_str());

	GLuint shader;
	std::string shaderAsText;
	std::ifstream inFile(shaderFileName);
	if (inFile.is_open()) {
		
		// Get length by seeking the end
		inFile.seekg(0, std::ios::end);

		// Reserve the number of bytes 
		shaderAsText.reserve(inFile.tellg());

		// Start from the beginning
		inFile.seekg(0, std::ios::beg);

		// read contents of the file and assign it to destination string
		shaderAsText.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
		inFile.close();
	}
	else {
		Logger::log(0, " %s : Error - Failed to open File %s.\n", __FUNCTION__,shaderFileName.c_str());
		Logger::log(1, "%s error: system says '%s'\n", __FUNCTION__, strerror(errno));
		return 0;
	}

	if (inFile.bad() || inFile.fail()) {
		inFile.close();
		Logger::log(0, " %s : Error - Read failed or bad state for File %s.\n", __FUNCTION__, shaderFileName.c_str());
		return 0;
	}
	inFile.close();

	const char* shaderSource = shaderAsText.c_str();

	// Create memory for shader type
	shader = glCreateShader(shaderType);

	// Load shader code into shader
	glShaderSource(shader, 1, (const GLchar**)&shaderSource, 0);

	// Compile shader
	glCompileShader(shader);

	// Check if compile was successful
	GLint isShaderCompiled;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &isShaderCompiled);

	if (!isShaderCompiled) {

		Logger::log(0, " %s : Error - Could not compile shader.\n", __FUNCTION__);
		return 0;
	}

	Logger::log(1, " %s : Read Shader %s successfully.\n", __FUNCTION__, shaderFileName.c_str());

	return shader;
}

void Shader::use() {

	Logger::log(2, " %s : Using Shader.\n", __FUNCTION__);

	glUseProgram(mShaderProgram);
}

bool Shader::getUniformLocation(std::string uniformName) 
{
	if (mShaderProgram > 0)
	{
		mUniformLocation = glGetUniformLocation(mShaderProgram, uniformName.c_str());
		return mUniformLocation > -1;
	}
	return false;
}

void Shader::setUniformValue(int value)
{
	if (mShaderProgram > 0) 
	{
		if (mUniformLocation > -1)
		{
			glUniform1i(mUniformLocation, value);
		}
	}
}

