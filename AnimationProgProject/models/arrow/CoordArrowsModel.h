/* coordinate arrows */
#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "../../opengl/mainRenderer/OGLRenderData.h"

class CoordArrowsModel {
public:
    OGLMesh getVertexData();

private:
    void init();
    OGLMesh mVertexData;
};
