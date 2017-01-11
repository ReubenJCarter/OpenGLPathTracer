#include <iostream>
#include <vector> 
#include <algorithm>
#include <limits>
#include <math.h>

#include "RenderEngine/Image.h"
#include "RenderEngine/Scene.h"


int main(int argc, char* argv[])
{
	if(argc < 2)
		return 0;
	
	RenderEngine::Scene scene(argv[1]);
	RenderEngine::BVH bvh;
	bvh.Build(scene.triangles, scene.verticies, 4);
	bvh.ToFile("test.bvh");
	RenderEngine::Image image(512, 512);
	image.ToFile("test.png");
	
	std::cout << "DONE";
	
	return 1;
}