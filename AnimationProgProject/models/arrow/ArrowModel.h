#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "./../../opengl/mainRenderer/OGLRenderData.h"

class ArrowModel {
public:
    OGLMesh getVertexData();

private:
    void init();
    OGLMesh mVertexData;
};