#include "Window.h"
#include "../Logger/Logger.h"
#include <vector>

bool Window::init(unsigned int width, unsigned int height, std::string title) {

	// If failed to init glfw
	if (!glfwInit()) {
		Logger::log(1, "%s: glfwInit() Error.\n", __FUNCTION__);
		return false;
	}

	// If no vulkan support
	if (!glfwVulkanSupported()) {
		glfwTerminate();
		Logger::log(1, "%s: Vulkan is not supported.\n", __FUNCTION__);
		return false;
	}

	// Setting properties or "Hints" for the next window to be created.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	mApplicationName = title;

	// Vulkan needs no context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Try to create window
	mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	// If window creation failed then terminate and exit
	if (!mWindow) {
	
		Logger::log(1, "%s: Could not create window.\n", __FUNCTION__);
		glfwTerminate();
		return false;
	}

	// If vulkan init failed 
	if (!initVulkan()) {
		glfwTerminate();
		Logger::log(1, "%s: Could not initialize Vulkan.\n", __FUNCTION__);
		return 0;
	}
	// Get OpenGL context and set it to current thread to have access to global state for rendering (Remove for vulkan since it need no context)
	//glfwMakeContextCurrent(mWindow);


	Logger::log(1, "%s: Window was successfully initialized.\n", __FUNCTION__);
	return true;

}

bool Window::initVulkan() {

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
		Logger::log(1, "%s: Error - No vulkan extensions found.\n", __FUNCTION__);
		return false;
	}

	Logger::log(1, "%s: Found %u Vulkan extensions\n", __FUNCTION__, extensionCount);


	VkInstanceCreateInfo createInfo{}; 

	// Set Struct type
	createInfo.sType =VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	// Point to any other struct types if needed
	createInfo.pNext = nullptr;

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
		Logger::log(1, "%s: Could not create Instance(% i)\n", __FUNCTION__, result);
		return false;
	}

	uint32_t physicalDeviceCount = 0;

	// Get number of GPUs that support vulkan
	vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);

	// if no supported GPUs found
	if (physicalDeviceCount == 0) {
		Logger::log(1, "%s: Error - No vulkan capable GPU found\n", __FUNCTION__);
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
		Logger::log(1, "%s: Could not create Vulkan surface\n", __FUNCTION__);
		return false;
	}

	return true;
}



void Window::mainLoop() {

	// Check wether window is initialized
	if (!mWindow) {
		Logger::log(1, "%s: No window initialized. try calling the init method first.\n", __FUNCTION__);
		return;
	}

	// Activate wait for vertical sync or else window will start to flicker 
	glfwSwapInterval(1);

	float color = 0.0f;


	// While the user has not generated the close window event.
	while (!glfwWindowShouldClose(mWindow)) {
		
		// Slowly increment value of color and reset when reaches to 1
		color >= 1.0f ? color = 0.0f : color += 0.01f;
		
		// Back Buffer is "cleared" by replacing current values with parameter values
		glClearColor(color, color, color, 1.0f);

		// Back buffer will give the screen the color set in buffer. 
		glClear(GL_COLOR_BUFFER_BIT);

		// Swap the front buffer with the back buffer to show its contents (Now the front will now become the back buffer after swapping)
		glfwSwapBuffers(mWindow);

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

	// Destroy Vulkan Surface
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

	// Destroy Vulkan Instance
	vkDestroyInstance(mInstance, nullptr);

	// Destroy Window
	glfwDestroyWindow(mWindow);

	glfwTerminate();
}