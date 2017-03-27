

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
target_link_libraries(RenderEngine opengl32)
target_link_libraries(RenderEngine glew32s)
target_link_libraries(RenderEngine glfw3)
target_link_libraries(RenderEngine GLComputeHelper)
target_link_libraries(RenderEngine assimp-vc140-mt)
target_link_libraries(RenderEngine DevIL)
