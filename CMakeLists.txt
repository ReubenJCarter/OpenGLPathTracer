cmake_minimum_required(VERSION 2.6)
project(GPUPathTracing)
set(PROJECT_BRIEF "GPUPathTracing")
set(CMAKE_BUILD_TYPE DEBUG)
set(GCC_COMPILE_FLAGS "-std=c++11 -Wall -O3 -g")
set(GCC_LINK_FLAGS "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}")

#ASSIMP

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/assimp-3.3.1/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/assimp-3.3.1/build/code/Release)

#DEVIL

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/DevIL/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/DevIL)

#GLM

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/glm)

#GLFW

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw-3.2.1/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw-3.2.1/build/src/Release)

#GLEW

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/glew-2.0.0/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/glew-2.0.0/lib/Release/x64)
add_definitions(-DGLEW_STATIC)

#GLComputeHelper

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/GLComputeHelper/src)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/GLComputeHelper/build/src/Release)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src)
add_subdirectory(test)