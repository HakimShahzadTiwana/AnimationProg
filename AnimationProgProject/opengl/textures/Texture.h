#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture {
	public:

		// Loads the texture and generates an openGL texture 
		// @param Filepath to texture
		bool loadTexture(std::string textureFileName);


		// Enables modifications to the texture (For when we want to use multiple textures and avoid any unexpected results)
		void bind();

		// Disables modifications to the texture (For when we want to use multiple textures and avoid any unexpected results)
		void unbind();

		// Cleans up the texture
		void cleanup();

private:

	// Stores the generated openGL that was loaded.
	GLuint mTex = 0;


};