#include <string>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
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
	ImGui::Text(std::to_string(renderData.rdUIGenerateTime).c_str());
	ImGui::SameLine();
	ImGui::Text("ms");
	ImGui::Separator();


	/* Create text widget for camera data */
	ImGui::Text("View Azimuth:");
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string
	(renderData.rdViewAzimuth).c_str());
	ImGui::Text("View Elevation:");
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(renderData.rdViewElevation).c_str());
	ImGui::Text("Camera Position:");
	ImGui::SameLine();
	ImGui::Text("%s", glm::to_string(renderData.rdCameraWorldPosition).c_str());
	ImGui::Separator();

	/* Slerp + Spline Section */

	if (ImGui::CollapsingHeader("Slerp + Spline"))
	{
		ImGui::Indent();


		if (ImGui::Button("Reset All"))
		{
			renderData.rdResetAnglesAndInterp = true;
		}

		ImGui::Text("Interpolate");
		ImGui::SameLine();
		ImGui::SliderFloat("##Interp", &renderData.rdInterpValue, 0.0f, 1.0f);

		if (ImGui::CollapsingHeader("Slerp"))
		{
			ImGui::Checkbox("Draw World Coordinate Arrows", &renderData.rdDrawWorldCoordArrows);
			ImGui::Checkbox("Draw Model Coordinate Arrows", &renderData.rdDrawModelCoordArrows);

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("X Rotation ");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SliderInt2("##ROTX", renderData.rdRotXAngle.data(), 0, 360);

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
			ImGui::Text("Y Rotation ");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SliderInt2("##ROTY", renderData.rdRotYAngle.data(), 0, 360);

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
			ImGui::Text("Z Rotation ");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SliderInt2("##ROTZ", renderData.rdRotZAngle.data(), 0, 360);
		}

		if (ImGui::CollapsingHeader("Spline"))
		{
			ImGui::Checkbox("Draw spline lines", &renderData.rdDrawSplineLines);

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
			ImGui::Text("Start Vec ");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SliderFloat3("##STARTVEC", glm::value_ptr(renderData.rdSplineStartVertex), -10.0f, 10.0f);

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
			ImGui::Text("Start Tang ");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SliderFloat3("##STARTTANG", glm::value_ptr(renderData.rdSplineStartTangent), -10.0f, 10.0f);

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
			ImGui::Text("End Vec ");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SliderFloat3("##ENDVEC", glm::value_ptr(renderData.rdSplineEndVertex), -10.0f, 10.0f);

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
			ImGui::Text("End Tang ");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SliderFloat3("##ENDTANG", glm::value_ptr(renderData.rdSplineEndTangent), -10.0f, 10.0f);
		}
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("glTF Animation")) 
	{
		ImGui::Text("Clip No");
		ImGui::SameLine();
		ImGui::SliderInt("##Clip", &renderData.rdAnimClip, 0, renderData.rdAnimClipSize - 1);
		ImGui::Text("Clip Name: %s", renderData.rdClipName.c_str());
		ImGui::Checkbox("Play Animation", &renderData.rdPlayAnimation);
		if (!renderData.rdPlayAnimation)
		{
			ImGui::BeginDisabled();
		}
		ImGui::Text("Speed ");
		ImGui::SameLine();
		ImGui::SliderFloat("##ClipSpeed", &renderData.rdAnimSpeed, 0.0f, 2.0f);
		if (!renderData.rdPlayAnimation)
		{
			ImGui::EndDisabled();
		}
		if (renderData.rdPlayAnimation)
		{
			ImGui::BeginDisabled();
		}
		ImGui::Text("Timepos");
		ImGui::SameLine();
		ImGui::SliderFloat("##ClipPos",&renderData.rdAnimTimePosition, 0.0f,renderData.rdAnimEndTime);
		if (renderData.rdPlayAnimation) 
		{
			ImGui::EndDisabled();
		}

		if (ImGui::CollapsingHeader("glTF Animation Blending"))
		{
			ImGui::Checkbox("Blending Type:",&renderData.rdCrossBlending);
			ImGui::SameLine();
			if (renderData.rdCrossBlending) 
			{
				ImGui::Text("Cross");
			}
			else 
			{
				ImGui::Text("Single");
			}
			if (renderData.rdCrossBlending) 
			{
				ImGui::BeginDisabled();
			}

			ImGui::Text("Blend Factor");
			ImGui::SameLine();
			ImGui::SliderFloat("##BlendFactor",&renderData.rdAnimBlendFactor, 0.0f, 1.0f);
			if (renderData.rdCrossBlending) 
			{
				ImGui::EndDisabled();
			}
			if (!renderData.rdCrossBlending) 
			{
				ImGui::BeginDisabled();
			}
			ImGui::Text("Dest Clip ");
			ImGui::SameLine();
			ImGui::SliderInt("##DestClip", &renderData.rdCrossBlendDestAnimClip, 0,renderData.rdAnimClipSize - 1);
			ImGui::Text("Dest Clip Name: %s",renderData.rdCrossBlendDestClipName.c_str());
			ImGui::Text("Cross Blend ");
			ImGui::SameLine();
			ImGui::SliderFloat("##CrossBlendFactor", &renderData.rdAnimCrossBlendFactor, 0.0f, 1.0f);
			if (!renderData.rdCrossBlending)
			{
				ImGui::EndDisabled();
			}
		}
	}

	
	/* Create text for triangle count */

	// Print text in new line 
	ImGui::Text("Triangles: ");
	// Tell imgui to stay on the same line for the next text
	ImGui::SameLine();
	// Convert the triangle count to c string and display text on the same line
	ImGui::Text(std::to_string(renderData.rdTriangleCount + renderData.rdGltfTriangleCount).c_str());

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
	
	
	ImGui::Checkbox("Enable GPU Vertex Skinning", &renderData.rdGPUVertexSkinning);

	if (renderData.rdGPUVertexSkinning)
	{
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::Text("Enabled");
		ImGui::PopStyleColor();
	}


	ImGui::Checkbox("Enable GPU Dual Quat Vertex Skinning", &renderData.rdGPUDualQuatVertexSkinning);

	if (renderData.rdGPUDualQuatVertexSkinning)
	{
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::Text("Enabled");
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
