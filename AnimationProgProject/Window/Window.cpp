#include "Window.h"
#include "../Logger/Logger.h"
#include <vector>

bool Window::init(unsigned int width, unsigned int height, std::string title) {

	// If failed to init glfw
	if (!glfwInit()) {
		Logger::log(0, "%s: Error - glfwInit() failed.\n", __FUNCTION__);
		return false;
	}

	// If no vulkan support
	if (!glfwVulkanSupported()) {
		glfwTerminate();
		Logger::log(0, "%s: Error - Vulkan is not supported.\n", __FUNCTION__);
		return false;
	}

	// Setting properties or "Hints" for the next window to be created.
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

	
	mApplicationName = title;

	// Vulkan needs no context
	// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Try to create window
	mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	// If window creation failed then terminate and exit
	if (!mWindow) {
	
		Logger::log(0, "%s: Error - Could not create window.\n", __FUNCTION__);
		glfwTerminate();
		return false;
	}

	// If vulkan init failed 
	if (!initVulkan()) {
		glfwTerminate();
		Logger::log(0, "%s: Error - Could not initialize Vulkan.\n", __FUNCTION__);
		return 0;
	}
	// Get OpenGL context and set it to current thread to have access to global state for rendering (Remove for vulkan since it need no context)
	glfwMakeContextCurrent(mWindow);

	// OpenGL Renderer
	//mRenderer = std::make_unique<OGLRenderer>();

	// Vulkan Renderer
	mRenderer = std::make_unique<VkRenderer>(mWindow);

	if (!mRenderer->init(width, height)) {

		Logger::log(0, "%s: Error - Could not init Renderer.\n", __FUNCTION__);

		glfwTerminate();

		return false;
	}

	// Associates any type of pointer (in this case the this pointer) to a window
	glfwSetWindowUserPointer(mWindow, this);

	// Creating a callback using a lambda function "[]" so that when the window closes the handle Window close event runs
	glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* win) {
		auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
		thisWindow->handleWindowCloseEvents();
	});

	// Creating a callback function for keyboard events
	glfwSetKeyCallback(mWindow, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
		auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
		thisWindow->handleKeyEvents(key, scancode, action, mods);
	});

	// Creating a callback function for mouse button events
	glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* win, int button, int action, int mods) {
		auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
		thisWindow->handleMouseButtonEvents(button, action, mods);
	});


	// Set user pointer to renderer for resizing
	glfwSetWindowUserPointer(mWindow, mRenderer.get());

	// Resizing window sends callback to renderer to set new size
	glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* win, int width, int height) {
		auto renderer = static_cast<OGLRenderer*>(glfwGetWindowUserPointer(win));
		renderer->setSize(width, height);
	});
	
	mModel = std::make_unique<Model>();
	mModel->init();

	Logger::log(1, "%s: Window was successfully initialized.\n", __FUNCTION__);
	return true;

}

bool Window::initVulkan() {
/*
	VkResult result = VK_ERROR_UNKNOWN;

	// Struct that contains Information about the application that we will use later
	VkApplicationInfo appInfo{};

	// Set the Struct type as ApplicationInfo
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	// Points to another other structs that we want to use (null for now)
	appInfo.pNext = nullptr;

	// Set to minimum Vulkan API version that we want to use
	appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 1, 0);


	// Check to see if we have the required extensions to run vulkan
	uint32_t extensionCount = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	// if not extensions found
	if (extensionCount == 0) {
		Logger::log(0, "%s: Error - No vulkan extensions found.\n", __FUNCTION__);
		return false;
	}

	Logger::log(1, "%s: Found %u Vulkan extensions\n", __FUNCTION__, extensionCount);


	VkInstanceCreateInfo createInfo{}; 

	// Set Struct type
	createInfo.sType =VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	// Point to any other struct types if needed
	/createInfo.pNext = nullptr;

	// Set Application info
	createInfo.pApplicationInfo = &appInfo;

	// Set count of extensions
	createInfo.enabledExtensionCount = extensionCount;

	// Set names of extensions
	createInfo.ppEnabledExtensionNames = extensions;

	// Set count of enabled layers
	createInfo.enabledLayerCount = 0;

	// The instance includes the storage for the Vulkan state on the application level, 
	// and there is no longer a system global state(“context”) like in OpenGL
	result = vkCreateInstance(&createInfo, nullptr, &mInstance);

	// if failed to create instance
	if ( result != VK_SUCCESS) {
		Logger::log(0, "%s: Error - Could not create Instance(% i)\n", __FUNCTION__, result);
		return false;
	}

	uint32_t physicalDeviceCount = 0;

	// Get number of GPUs that support vulkan
	vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);

	// if no supported GPUs found
	if (physicalDeviceCount == 0) {
		Logger::log(0, "%s: Error - No vulkan capable GPU found\n", __FUNCTION__);
		return false;
	}

	Logger::log(1, "%s: Found %u physical device(s)\n", __FUNCTION__, physicalDeviceCount);

	std::vector<VkPhysicalDevice> devices;

	// Store GPU data
	vkEnumeratePhysicalDevices(mInstance,&physicalDeviceCount, devices.data());
	
	

	// Create vulkan surface
	result = glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface);

	// if surface failed to be created
	if(result != VK_SUCCESS) {
		Logger::log(0, "%s: Error - Could not create Vulkan surface\n", __FUNCTION__);
		return false;
	}
	*/
	return true;

}

void Window::mainLoop() {

	// Check wether window is initialized
	if (!mWindow) {
		Logger::log(0, "%s: Error - No window initialized. try calling the init method first.\n", __FUNCTION__);
		return;
	}

	// Activate wait for vertical sync or else window will start to flicker 
	glfwSwapInterval(1);

	mRenderer->uploadData(mModel->getVertexData());

	// float color = 0.0f;

	Logger::log(1, "%s: Starting window loop...\n", __FUNCTION__);
	// While the user has not generated the close window event.
	while (!glfwWindowShouldClose(mWindow)) {
		/* Being done by renderer now 
		* 
		// Slowly increment value of color and reset when reaches to 1
		color >= 1.0f ? color = 0.0f : color += 0.01f;
		
		// Back Buffer is "cleared" by replacing current values with parameter values
		glClearColor(color, color, color, 1.0f);

		// Back buffer will give the screen the color set in buffer. 
		glClear(GL_COLOR_BUFFER_BIT);

		*/


		if (!mRenderer->draw()) {
			Logger::log(0, "%s: Error - Failed to draw from Renderer", __FUNCTION__);
		}
		// Swap the front buffer with the back buffer to show its contents (Now the front will now become the back buffer after swapping)
		glfwSwapBuffers(mWindow);

		// Poll events to react to anything that happens to the window. (Inputs, Button presses etc.)
		glfwPollEvents();
	}

	Logger::log(1, "%s: Window loop ended.\n", __FUNCTION__);
}

void Window::cleanup() {

	// Check wether window is initialized
	if (!mWindow) {
		Logger::log(0, "%s: Error - No window initialized. Try calling the init method first.\n", __FUNCTION__);
		return;
	}

	Logger::log(1, "%s: Terminating Window.\n",__FUNCTION__);

	// Destroy Vulkan Surface
	//vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

	// Destroy Vulkan Instance
	//vkDestroyInstance(mInstance, nullptr);

	// Destroy Window
	glfwDestroyWindow(mWindow);

	glfwTerminate();
}

void Window::handleWindowCloseEvents() {
	Logger::log(1, "%s: Window got close event... bye!\n", __FUNCTION__);
}

void Window::handleKeyEvents(int key, int scancode, int action, int mods) {
	std::string actionName;

	switch (action) {
	case GLFW_PRESS:
		actionName = "Pressed";
		break;

	case GLFW_RELEASE:
		actionName = "Released";
		break;
	
	case GLFW_REPEAT:
		actionName = "Held";
		break;
	
	default:
		actionName = "Invalid";
		break;
	}
	const char* keyName = glfwGetKeyName(key,0);
	Logger::log(1, " %s : key %s (key %i, scancode %i ) %s.\n", __FUNCTION__, keyName, key, scancode, actionName.c_str());
}

void Window::handleMouseButtonEvents(int button, int action, int mods) {
	std::string buttonName;
	std::string actionName;

	switch (action) {

	case GLFW_PRESS:
		actionName = "Pressed";
		break;

	case GLFW_RELEASE:
		actionName = "Released";
		break;

	default:
		actionName = "Invalid";
		break;
	}

	switch (button) {
	
	case GLFW_MOUSE_BUTTON_LEFT:
		buttonName = "Left button";
		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		buttonName = "Right button";
		break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
		buttonName = "Middle button";
		break;

	default:
		buttonName = "Other button";
		break;
	}
	Logger::log(1, "%s: %s mouse (%i) %s\n",__FUNCTION__, buttonName.c_str(), button,actionName.c_str());
}