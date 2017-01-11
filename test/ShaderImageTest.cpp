#include <iostream>
#include <vector>
#include <time.h>  

#include "GLComputeHelper/Context.h"
#include "GLComputeHelper/ComputeShader.h"
#include "RenderEngine/Image.h"


using namespace GLComputeHelper;
using namespace RenderEngine;


struct Member
{
	float a;
	float b;
	float c;
};

int main()
{
	clock_t t;
	
	GLComputeHelper::Context::Init();
	GLComputeHelper::Context context;
	
	ComputeShader computeShader(
	"#version 430\n"
	"layout(rgba32f) uniform image2D anImage;"
	"layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;"
	"void main()"
	"{"
	"	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);"
	"	vec4 pix = vec4(float(gl_GlobalInvocationID.x) / 512.0f, float(gl_GlobalInvocationID.y) / 512.0f, 1.0f, 1.0f);"
	"	imageStore(anImage, storePos, pix);"
    "}"
	);
	
	t = clock();
	
	ShaderImage shaderImage(512, 512);
	computeShader.SetImage("anImage", shaderImage);
	
	computeShader.Dispatch(512 / 16, 512 / 16, 1);
	computeShader.Block();
	
	Image image(512, 512);
	shaderImage.Get(image.Data());
	
	t = clock() - t;
	std::cout << "Time Taken:" << ((float)t)/CLOCKS_PER_SEC << std::endl;

	image.ToFile("Test.png");
	
	GLComputeHelper::Context::Destroy();
	std::cout << "DONE" << std::endl;

	return 1;
}