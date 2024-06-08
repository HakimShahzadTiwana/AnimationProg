#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.h"
#include "../Logger/Logger.h"

bool Texture::loadTexture(std::string textureName) {

	Logger::log(1, "%s: Started loading texture...\n", __FUNCTION__);


	int texWidth, texHeight, numOfChannels;

	// Flips the image on the vertical axis (picture has origin at top-left while texture has it at bottom-left)
	stbi_set_flip_vertically_on_load(true);

	// Creates memory area, read the file, flips the image (since we set that to true) and fill the output params
	unsigned char* textureData = stbi_load(textureName.c_str(), &texWidth, &texHeight, &numOfChannels, 0);

	// If the texture wasn't able to load for some reason then free the allocated memory area or else memory leak
	if (!textureData) {

		Logger::log(0, "%s: Error - Something went wrong while loading the texture.\n", __FUNCTION__);
		stbi_image_free(textureData);
		return false;
	}
/*
	// For minification we use tri-linear sampling
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// For magnification we use linear filtering since thats all that iis available 
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// Repeat textures outside the range 0 to 1 
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Uses the byte data of the load function and pushes the data to the gpu from the memory 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	// Generate mipmaps (scaled down versions of original image, halving the width and height for every step until a configurable limit is reached)
	// increases rendering speed, as less data is read if texture is far away and reduces artifacts
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Free memory allocated by STB load
	stbi_image_free(textureData);

	Logger::log(1, "%s: Loaded texture successfully. \n", __FUNCTION__);

	return true;
}

void Texture::bind() {

	Logger::log(1, "%s: Binding Texture. \n", __FUNCTION__);

	glBindTexture(GL_TEXTURE_2D, mTex);
}
void Texture::unbind() {
	
	Logger::log(1, "%s: Unbinding Texture. \n", __FUNCTION__);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::cleanup() {

	Logger::log(1, "%s: Cleaning up Texture. \n", __FUNCTION__);

	mTex = 0;
}