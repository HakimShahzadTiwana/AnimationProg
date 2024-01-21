// AnimationProgProject.cpp : Defines the entry point for the application.
//

#include "AnimationProgProject.h"
#include <memory>
#include "Window/Window.h"
#include "Logger/Logger.h"

using namespace std;

int main(int argc, char *argv[])
{
	unique_ptr<Window> w = make_unique<Window>();

	// Try to initialize window.
	if (!w->init(640, 480, "First Window")) {
		Logger::log(0, "%s: Error - Window init Error.\n", __FUNCTION__);
		return -1;
	}

	// Start window main loop
	w->mainLoop();

	// After main loop is over cleanup program.
	w->cleanup();
	return 0;
}
