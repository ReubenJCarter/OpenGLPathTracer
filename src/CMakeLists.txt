#
#GLComputeHelper lib
#

set(GLHelperSrc
	GLComputeHelper/Context.cpp
	GLComputeHelper/ComputeShader.cpp
	GLComputeHelper/Util.cpp
	GLComputeHelper/ShaderImage.cpp
	GLComputeHelper/QuadImage.cpp
	GLComputeHelper/Texture.cpp
	GLComputeHelper/BufferTexture.cpp
)	

add_library(GLComputeHelper STATIC ${GLHelperSrc})

#link with gl specific libs
target_link_libraries(GLComputeHelper opengl32)
target_link_libraries(GLComputeHelper glew32s)
target_link_libraries(GLComputeHelper glfw3)


#
#Renderer lib
#

set(RenderEngineSrc
	RenderEngine/BVH.cpp
	RenderEngine/Camera.cpp
	RenderEngine/Image.cpp
	RenderEngine/Scene.cpp
	RenderEngine/DataTypes.cpp
	RenderEngine/Renderer.cpp
)	

add_library(RenderEngine STATIC ${RenderEngineSrc})

#link with compute helper lib and assimp devil
target_link_libraries(RenderEngine GLComputeHelper)
target_link_libraries(RenderEngine assimp-vc140-mt)
target_link_libraries(RenderEngine DevIL)