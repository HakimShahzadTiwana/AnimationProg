#include <string>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "UserInterface.h"

void UserInterface::init(OGLRenderData& renderData)
{
	// Required for proper init
	IMGUI_CHECKVERSION();
	//Searches for imgui.ini which contains setting of ImGuiv widgets, if .ini doesnt exsist it will create one 
	ImGui::CreateContext();

	// Init the Glfw backend using the GLFW window, and set true to create its own keyboard/mouse callbacks 
	ImGui_ImplGlfw_InitForOpenGL(renderData.rdWindow, true);

	// Init OpenGL backend with the same version shaders we're using
	const char* glslVersion = "#version 460 core";
	ImGui_ImplOpenGL3_Init(glslVersion);

}

void UserInterface::createFrame(OGLRenderData& renderData)
{
	// Create new frames in the backends and imgui 
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	/*Overlay window of imgui*/

	// Set to allow changing properties of the imgui window like for resizing 
	ImGuiWindowFlags imGuiWindowFlags = 0;

	// Set transparency of the window
	ImGui::SetNextWindowBgAlpha(0.8f);

	// Starts a new window 
	// Params - Title of window, pointer to bool that is set to true when window is closed, windowflags
	ImGui::Begin("Control", nullptr, imGuiWindowFlags);

	// Get FPS
	static float newFps = 0.0f;
	if (renderData.rdFrameTime > 0.0) {
		newFps = 1.0f / renderData.rdFrameTime;
	}
	framesPerSecond = (averagingAlpha * framesPerSecond) + (1.0f - averagingAlpha) * newFps;

	/*Text widget for FPS*/
	ImGui::Text("FPS:");
	ImGui::SameLine();
	ImGui::Text(std::to_string(framesPerSecond).c_str());
	//Add a horizontal line beneath the fps text 
	ImGui::Separator();

	/* Text Widget for UI Generation Time (CreateFrame) */
	ImGui::Text("UI Generation Time:");
	ImGui::SameLine();
	ImGui::Text(std::to_string
	(renderData.rdUIGenerateTime).c_str());
	ImGui::SameLine();
	ImGui::Text("ms");
	ImGui::Separator();

	/* Create text for triangle count */

	// Print text in new line 
	ImGui::Text("Triangles: ");
	// Tell imgui to stay on the same line for the next text
	ImGui::SameLine();
	// Convert the triangle count to c string and display text on the same line
	ImGui::Text(std::to_string(renderData.rdTriangleCount).c_str());

	// Add Text widget for window dimensions
	std::string windowDims = std::to_string(renderData.rdWidth) + "x" + std::to_string(renderData.rdHeight);
	ImGui::Text("Window Dimensions: ");
	ImGui::SameLine();
	ImGui::Text(windowDims.c_str());

	// Add Text widget for image window position by getting the position from imgui itself
	std::string imgWindowPos = std::to_string(static_cast<int>(ImGui::GetWindowPos().x)) + std::to_string(static_cast<int>(ImGui::GetWindowPos().y));
	ImGui::Text("ImGui Window Position: ");
	ImGui::SameLine();
	ImGui::Text(imgWindowPos.c_str());

	/* Create checkbox */
	static bool checkBoxChecked = false;
	ImGui::Checkbox("Check Me!", &checkBoxChecked);

	if (checkBoxChecked) 
	{
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::Text("Yes");
		ImGui::PopStyleColor();
	}

	/* Create Button for toggling shaders */
	if (ImGui::Button("Toggle Shader")) 
	{
		renderData.rdUseChangedShader = !renderData.rdUseChangedShader;
	}
	ImGui::SameLine();
	if (!renderData.rdUseChangedShader) 
	{
		ImGui::Text("Basic Shader");
	}
	else 
	{
		ImGui::Text("Changed Shader");
	}

	/* Slider for FOV */
	ImGui::Text("Field of View");
	ImGui::SameLine();

	// Slider
	// Params -  ## Disables trailing text, pointer to current value, min value , max value
	ImGui::SliderInt("##FOV", &renderData.rdFieldOfView, 40, 150);

	ImGui::End();





}

void UserInterface::render()
{
	// Draw the created widgets into the buffer and screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void UserInterface::cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
