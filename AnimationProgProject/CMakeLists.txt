# CMakeList.txt : CMake project for AnimationProgProject, include source and define
# project specific logic here.
#

# The first library to support vulkan shader compiler
cmake_minimum_required (VERSION 3.19)

# Setting min requirment for C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# use custom file to find libraries ( Adds the cmake folder inside the project folder to the list of locations CMake uses to
# store helper scripts for the find_package command)
if(WIN32)
 list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Project name, automatically adds the .exe extension when completed
project ("AnimationProgProject")

file(GLOB IMGUI_SRC  "imgui/*.cpp")
file(GLOB WINDOW_SRC "window/*.cpp" "window/*.h") 
file(GLOB LOGGER_SRC "logger/*.cpp" "logger/*.h")
file(GLOB OPENGL_SRC "opengl/*.cpp" "opengl/*.h")
file(GLOB INCLUDE_SRC "include/*.cpp" "include/*.h")
file(GLOB MODEL_SRC "models/*.cpp" "models/*.h")
file(GLOB UI_SRC "userInterface/*.cpp" "userInterface/*.h")
file(GLOB TINYGLTF "tinygltf/*")



# Add source to this project's executable 
add_executable (AnimationProgProject "AnimationProgProject.cpp" "AnimationProgProject.h" "src/glad.c" ${TINYGLTF} ${WINDOW_SRC} ${LOGGER_SRC} ${INCLUDE_SRC} ${MODEL_SRC} ${IMGUI_SRC} ${UI_SRC} "opengl/mainRenderer/OGLRenderer.cpp" "opengl/mainRenderer/OGLRenderer.h" "opengl/mainRenderer/OGLRenderData.h" "opengl/buffers/frameBuffer/FrameBuffer.h" "opengl/buffers/frameBuffer/FrameBuffer.cpp" "opengl/buffers/vertexBuffer/VertexBuffer.h" "opengl/buffers/vertexBuffer/VertexBuffer.cpp" "opengl/buffers/uniformBuffer/UniformBuffer.cpp" "opengl/buffers/uniformBuffer/UniformBuffer.h" "opengl/textures/Texture.h" "opengl/textures/Texture.cpp" "opengl/shaders/Shader.h" "opengl/shaders/Shader.cpp" "timer/Timer.h" "timer/Timer.cpp" "camera/Camera.h" "camera/Camera.cpp" "models/arrow/ArrowModel.cpp" "models/arrow/ArrowModel.h" "models/arrow/CoordArrowsModel.h" "models/arrow/CoordArrowsModel.cpp" "models/spline/SplineModel.h" "models/spline/SplineModel.cpp" "models/gltf/GltfModel.h" "models/gltf/GltfModel.cpp" "models/gltf/GltfNode.h" "models/gltf/GltfNode.cpp" "opengl/buffers/shaderStorageBuffer/ShaderStorageBuffer.h" "opengl/buffers/shaderStorageBuffer/ShaderStorageBuffer.cpp" "models/animations/GltfAnimationChannel.h" "models/animations/GltfAnimationChannel.cpp" "models/animations/GltfAnimationClip.h" "models/animations/GltfAnimationClip.cpp" "models/animations/IK/IKSolver.h" "models/animations/IK/IKSolver.cpp" "models/ModelSettings.h" "models/gltf/GltfInstance.h" "models/gltf/GltfInstance.cpp")


# Finds the glfw library and marks as required 
find_package(glfw3 3.3 REQUIRED)

# variable is set by FindGLFW3.cmake, reuse for Linux
if(UNIX)
 set(GLFW3_LIBRARY glfw)
endif()

# add the GLFW3 headers to the list of include paths for the compiler and the library to link to the final executable
include_directories(${GLFW3_INCLUDE_DIR})
target_link_libraries(AnimationProgProject ${GLFW3_LIBRARY})

# add for OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(AnimationProgProject ${GLFW3_LIBRARY} OpenGL::GL)

# add for Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(AnimationProgProject ${GLFW3_LIBRARY} Vulkan::Vulkan)

# add for glad loader
target_include_directories(AnimationProgProject PUBLIC include src window tools opengl model imgui tinygltf)


##Template code that i dont think i need
#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET AnimationProgProject PROPERTY CXX_STANDARD 20)
#endif()


