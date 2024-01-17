#include "Window.h"
#include "../Logger/Logger.h"

bool Window::init(unsigned int width, unsigned int height, std::string title) {
	if (!glfwInit()) {
		Logger::log(1, "%s: glfwInit() Error.\n", __FUNCTION__);
		return false;
	}

	// Setting properties or "Hints" for the next window to be created.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Try to create window
	mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	// If window creation failed then terminate and exit
	if (!mWindow) {
	
		Logger::log(1, "%s: Could not create window.\n", __FUNCTION__);
		glfwTerminate();
		return false;
	}

	Logger::log(1, "%s: Window was successfully initialized.\n ");
	return true;

}

void Window::mainLoop() {

	// Check wether window is initialized
	if (!mWindow) {
		Logger::log(1, "%s: No window initialized. try calling the init method first.\n", __FUNCTION__);
		return;
	}

	// While the user has not generated the close window event.
	while (!glfwWindowShouldClose(mWindow)) {

		// Poll events to react to anything that happens to the window. (Inputs, Button presses etc.)
		glfwPollEvents();

	}
}

void Window::cleanup() {

	// Check wether window is initialized
	if (!mWindow) {
		Logger::log(1, "%s: No window initialized. Try calling the init method first.\n", __FUNCTION__);
		return;
	}


	Logger::log(1, "%s: Terminating Window.\n",__FUNCTION__);

	glfwDestroyWindow(mWindow);
	glfwTerminate();
}