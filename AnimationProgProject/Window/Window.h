#pragma once

#include <string>
// Note : Vulkan include has to be before GLFW so that GLFW can detect Vulkan
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


class Window {
	public:
		// Initializes window with specified width,height and title
		bool init(unsigned int width, unsigned int height, std::string title);

		// Initialize Vulkan 
		bool initVulkan();

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
		VkInstance mInstance{};

		// Hnalde for Vulkan Surface (The drawable "Surface")
		VkSurfaceKHR mSurface{};

};			