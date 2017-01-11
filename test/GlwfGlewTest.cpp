#include <iostream>
#include <vector>
#include <time.h>  

#include "GLComputeHelper/Context.h"
#include "GLComputeHelper/ComputeShader.h"


using namespace GLComputeHelper;


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
	"struct Member"
	"{"
	"	float a;"
	"	float b;"
	"	float c;"
	"};"
	"layout (std430) buffer aStorageBuffer"
	"{"
	"	Member sb[];"
	"};"
	
	"layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;"
	"void main()"
	"{"
	"	sb[gl_GlobalInvocationID.x].c += sb[gl_GlobalInvocationID.x].a * sb[gl_GlobalInvocationID.x].b;"
    "}"
	);
	
	
	int problemSize = 10000;
	std::vector<Member> aStorageBufferVector(problemSize);
	
	
	for(int i = 0; i < problemSize; i++)
	{
		aStorageBufferVector[i].a = (float)i;
		aStorageBufferVector[i].b = 2* (float)i;
		aStorageBufferVector[i].c = 0.0f;
	}
	
	t = clock();
	
	StorageBuffer<Member> aStorageBuffer(problemSize, 1);
	aStorageBuffer.Copy(&aStorageBufferVector[0]);
	computeShader.SetStorageBuffer<Member>("aStorageBuffer", aStorageBuffer);
	
	computeShader.Dispatch(problemSize / 1024, 1, 1);
	computeShader.Block();
	
	aStorageBuffer.Get(&aStorageBufferVector[0]);
	
	t = clock() - t;
	std::cout << "Time Taken:" << ((float)t)/CLOCKS_PER_SEC << std::endl;
	
	for(int i = 0; i < 10; i++)
	{
		std::cout << "c=" << aStorageBufferVector[i].c << std::endl;
	}
	
	GLComputeHelper::Context::Destroy();
	std::cout << "DONE" << std::endl;

	return 1;
}