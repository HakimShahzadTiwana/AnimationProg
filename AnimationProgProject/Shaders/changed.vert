#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec4 texColor;
layout (location = 1) out vec2 texCoord;

void main() {

// offset for texture to be drawn a a bit upward and right for the new shader
  vec3 offset = vec3(0.1, 0.1, 0.1);
  gl_Position = vec4(aPos+offset, 1.0);
  texColor = vec4(aColor,1.0);
  texCoord = aTexCoord;
}
