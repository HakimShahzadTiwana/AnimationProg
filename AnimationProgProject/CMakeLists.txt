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

# Add source to this project's executable 
add_executable (AnimationProgProject "AnimationProgProject.cpp" "AnimationProgProject.h" "Window/Window.cpp" "Window/Window.h" "Logger/Logger.h" "Logger/Logger.cpp")


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


##Template code that i dont think i need
#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET AnimationProgProject PROPERTY CXX_STANDARD 20)
#endif()


