#pragma once
#include "./../opengl/mainRenderer/OGLRenderData.h"


class UserInterface {
public:
	void init(OGLRenderData& renderData);
	void createFrame(OGLRenderData& renderData);
	void render();
	void cleanup();

private:
	float framesPerSecond = 0.0f;
	float averagingAlpha = 0.96f;
};



