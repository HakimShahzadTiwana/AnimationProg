#pragma once

#include <string>
// Note : Vulkan include has to be before GLFW so that GLFW can detect Vulkan

#include <memory>
#include "./mainRenderer/OGLRenderer.h"
#include "../models/Model.h"
//#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


class Window {
	public:
		// Initializes window with specified width,height and title
		bool init(unsigned int width, unsigned int height, std::string title);

		// Opens the window and keeps the application running until closed down manually
		void mainLoop();

		// Preforms cleanup to shut down the application properly
		void cleanup();

		


	private:

		// Reference to the GLFW window
		GLFWwindow* mWindow = nullptr;

		// Stores name of application
		std::string mApplicationName;

		// Handle for Vulkan instance (Stores info about Vulkan settings for current application)
		//VkInstance mInstance{};

		// Handle for Vulkan Surface (The drawable "Surface")
		//VkSurfaceKHR mSurface{};

		std::unique_ptr<OGLRenderer> mRenderer;
		std::unique_ptr<Model> mModel;

		// To handle window close 
		void handleWindowCloseEvents();

		// To handle Keyboard events
		// @param key - ASCII code
		// @param scanCode - system-specific code
		// @param action - press,release,repeat(hold)
		// @param mods - shift,ctrl etc.
		void handleKeyEvents(int key, int scancode, int action, int mods);


		// To handle mouse button events
		// @param button - left,right,middle ect.
		// @param action - pressed,released
		// @param mods - shift,ctrl etc.
		void handleMouseButtonEvents(int button, int action, int mods);

};		

