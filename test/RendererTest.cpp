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
	
	int sizeX = 1280;
	int sizeY = 720;
	
	Context context(sizeX, sizeY);
	
	Scene scene(argv[1]);
	std::string cubeIm[] = {"cm_left.jpg", "cm_right.jpg", "cm_top.jpg", "cm_bottom.jpg", "cm_front.jpg", "cm_back.jpg"};
	scene.BackgroundCubeImageFromFile(cubeIm);
	
	std::cout << "Loaded scene file" << std::endl;
	
	scene.RebuildBVH(64);
	scene.bvh.ToFile("test.bvh");
	
	std::cout << "Rebuilt BVH" << std::endl;
	
	scene.AllocateGPUBuffers();
	scene.AllocateGPUBufferTextures();
	
	std::cout << "Allocated GPU Buffers" << std::endl;
	
	Renderer renderer(sizeX, sizeY, 5, 1);
	
	std::cout << "Created renderer" << std::endl;
	
	QuadImage quadImage(sizeX, sizeY);
	
	//BUNNY
	//scene.camera.position = glm::vec3(-0.05f, 0.1f, 0.17f);
	//scene.camera.rotation = glm::vec3(0.0f, 3.141f, 0.0f);
	scene.camera.fov = 1.1;
	//DRAGON
	scene.camera.position = glm::vec3(0.0f, 0.65f, 2.0f);
	scene.camera.rotation = glm::vec3(0.0f, 3.141f, 0.0f);
	
	if(argc >= 8)
	{
		std::stringstream ss;
		ss << argv[2] << " " << argv[3] << " " << argv[4] << " ";
		ss << argv[5] << " " << argv[6] << " " << argv[7] << " ";
		ss >> scene.camera.position.x >> scene.camera.position.y >> scene.camera.position.z;
		ss >> scene.camera.rotation.x >> scene.camera.rotation.y >> scene.camera.rotation.z;
	}
	
	double moveSpeed = 0.01f;
	
	int sampleCount = 1;
	clock_t t;
	
	while(true)
	{
		t = clock();
				
		renderer.Render(scene);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		quadImage.SetIntensity(1.0f / (float)sampleCount);
		quadImage.Draw(renderer.GetTexture());
		context.SwapBuffers();
		context.PollEvents();
		
		sampleCount++;
		
		//Movement
		GLFWwindow* window = context.GetGLFWWindow();
		int state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
		if (state == GLFW_PRESS)
		{
			moveSpeed = 0.03f;
		}
		else
		{
			moveSpeed = 0.01f;
		}
		state = glfwGetKey(window, GLFW_KEY_W);
		if (state == GLFW_PRESS)
		{
			scene.camera.position.z += moveSpeed;
			renderer.ClearTarget();
			sampleCount = 1;
		}
		state = glfwGetKey(window, GLFW_KEY_S);
		if (state == GLFW_PRESS)
		{
			scene.camera.position.z -= moveSpeed;
			renderer.ClearTarget();
			sampleCount = 1;
		}
		state = glfwGetKey(window, GLFW_KEY_D);
		if (state == GLFW_PRESS)
		{
			scene.camera.position.x += moveSpeed;
			renderer.ClearTarget();
			sampleCount = 1;
		}
		state = glfwGetKey(window, GLFW_KEY_A);
		if (state == GLFW_PRESS)
		{
			scene.camera.position.x -= moveSpeed;
			renderer.ClearTarget();
			sampleCount = 1;
		}
		state = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
		if (state == GLFW_PRESS)
		{
			scene.camera.position.y -= moveSpeed;
			renderer.ClearTarget();
			sampleCount = 1;
		}
		state = glfwGetKey(window, GLFW_KEY_SPACE);
		if (state == GLFW_PRESS)
		{
			scene.camera.position.y += moveSpeed;
			renderer.ClearTarget();
			sampleCount = 1;
		}
		state = glfwGetKey(window, GLFW_KEY_E);
		if (state == GLFW_PRESS)
		{
			scene.camera.rotation.y += moveSpeed;
			renderer.ClearTarget();
			sampleCount = 1;
		}
		state = glfwGetKey(window, GLFW_KEY_Q);
		if (state == GLFW_PRESS)
		{
			scene.camera.rotation.y -= moveSpeed;
			renderer.ClearTarget();
			sampleCount = 1;
		}
		
		t = clock() - t;
		double frameTime = (double)t / (double)CLOCKS_PER_SEC;
		std::cout << "Render Sample:" << sampleCount << " time:" << frameTime  << std::endl;
	}
}