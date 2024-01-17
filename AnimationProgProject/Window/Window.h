#pragma once

#include <string>
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
};