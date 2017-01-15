#include <iostream>
#include <sstream>
#include <vector>
#include <time.h>  

#include "GLComputeHelper/Context.h"
#include "GLComputeHelper/QuadImage.h"
#include "RenderEngine/Image.h"
#include "RenderEngine/Scene.h"
#include "RenderEngine/Renderer.h"

using namespace GLComputeHelper;
using namespace RenderEngine;

int main(int argc, char* argv[])
{
	if(argc < 2)
		return 0;
	
	Context::Init();
	
	int size = 512;
	
	Context context(size, size);
	
	Scene scene(argv[1]);
	std::string cubeIm[] = {"cm_left.jpg", "cm_right.jpg", "cm_top.jpg", "cm_bottom.jpg", "cm_front.jpg", "cm_back.jpg"};
	scene.BackgroundCubeImageFromFile(cubeIm);
	
	std::cout << "Loaded scene file" << std::endl;
	
	scene.RebuildBVH(32);
	scene.bvh.ToFile("test.bvh");
	
	std::cout << "Rebuilt BVH" << std::endl;
	
	scene.AllocateGPUBuffers();
	
	std::cout << "Allocated GPU Buffers" << std::endl;
	
	Renderer renderer(size, size, 3);
	
	std::cout << "Created renderer" << std::endl;
	
	QuadImage quadImage(size, size);
	
	//BUNNY
	scene.camera.position = glm::vec3(-0.05f, 0.1f, 0.17f);
	scene.camera.rotation = glm::vec3(0.0f, 3.141f, 0.0f);
	scene.camera.fov = 1.1;
	//DRAGON
	//scene.camera.position = glm::vec3(0.0f, 3.0f, -10.0f);
	//scene.camera.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	
	if(argc >= 8)
	{
		std::stringstream ss;
		ss << argv[2] << " " << argv[3] << " " << argv[4] << " ";
		ss << argv[5] << " " << argv[6] << " " << argv[7] << " ";
		ss >> scene.camera.position.x >> scene.camera.position.y >> scene.camera.position.z;
		ss >> scene.camera.rotation.x >> scene.camera.rotation.y >> scene.camera.rotation.z;
	}
	
	int sampleCount = 1;
	while(true)
	{
		std::cout << "Rendering Sampel:" << sampleCount << std::endl;
		
		renderer.Render(scene);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		quadImage.SetIntensity(1.0f / (float)sampleCount);
		quadImage.Draw(renderer.GetTexture());
		context.SwapBuffers();
		context.PollEvents();
		
		sampleCount++;
		
	}
}