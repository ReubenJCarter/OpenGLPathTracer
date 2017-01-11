#include <iostream>
#include <vector>
#include <time.h>  

#include "GLComputeHelper/Util.h"
#include "GLComputeHelper/Context.h"
#include "GLComputeHelper/ComputeShader.h"
#include "RenderEngine/Image.h"
#include "RenderEngine/Scene.h"


using namespace GLComputeHelper;
using namespace RenderEngine;


int main(int argc, char* argv[])
{
	if(argc < 2)
		return 0;
	
	GLComputeHelper::Context::Init();
	GLComputeHelper::Context context;
	
	ComputeShader computeShader(
	"\n#version 430\n"
	"\n#define PI 3.14159265359\n"
	"layout(rgba32f) uniform image2D outputImage;"
	
	"struct Vertex"
	"{"
	"	vec4 pos;"
	"	vec4 norm;"
	"	vec4 binorm;"
	"	vec4 tang;"
	"	vec4 uv;"
	"};"
	"layout (std430) buffer verticiesSB"
	"{"
	"	Vertex verticies[];"
	"};"
	
	"struct Triangle"
	"{"
	"	int a;"
	"	int b;"
	"	int c;"
	"	int mat;"
	"};"
	"layout (std430) buffer trianglesSB"
	"{"
	"	Triangle triangles[];"
	"};"
	
	"struct BVHNode"
	"{"
	"	vec4 min;"
	"	vec4 max;"
	"	ivec4 offsetAndSize;"
	"};"
	"layout (std430) buffer bvhSB"
	"{"
	"	BVHNode bvh[];"
	"};"
	
	"uniform int triangleCount;"
	"uniform int nodeCount;"
	"uniform float randTime0;"
	"uniform float randTime1;"
	"uniform int bounceCount;"
	"uniform int sampleNumber;"
	"uniform vec3 pointLightPosition;"
	"uniform vec3 pointLightColor;"
	
	"float RayIntersectTri(vec3 orig, vec3 dir, vec3 va, vec3 vb, vec3 vc, out vec4 hit)"
	"{"
	"	vec3 p0 = va;"
	"	vec3 e0 = vb - va;"
	"	vec3 e1 = vc - va;"
	"	vec3 pv = cross(dir, e1);"
	"	float det = dot(e0, pv);"
	"	vec3 tv = orig - p0;"
	"	vec3 qv = cross(tv, e0);"
	"	vec4 uvwt;"
	"	uvwt.y = dot(tv, pv);"
	"	uvwt.z = dot(dir, qv);"
	"	uvwt.x = dot(e1, qv);"
	"	uvwt.xyz = uvwt.xyz / det;"
	"	uvwt.w = uvwt.x;"
	"	uvwt.x = 1.0f - uvwt.y - uvwt.z;"
	"	if (all(greaterThanEqual(uvwt, vec4(0.0f)))) {"
	"		hit = uvwt;"
	"		return uvwt.w;"
	"	}"
	"	else"
	"		return 2e10;"
	"}"
	
	"bool RayIntersectBox(vec3 orig, vec3 dir, vec3 bbmin, vec3 bbmax, float t)"
	"{"
	"	vec3 t1 = (bbmin - orig) / dir;"
	"	vec3 t2 = (bbmax - orig) / dir;"
	"	float tmin = max(max(min(t1.x, t2.x), min(t1.y, t2.y)), min(t1.z, t2.z));"
	"	float tmax = min(min(max(t1.x, t2.x), max(t1.y, t2.y)), max(t1.z, t2.z));"
	"	return tmax >= max(0.0f, tmin) && tmin < t;"
	"}"
	
	"void RayIntersectBVH(vec3 rayOrig, vec3 rayDir, out int triIndex, out vec4 hit)"
	"{"
	"	float dist = 1e10;"
	"	int nodeIndex = 0;"
	"	int t = -1;"
	"	while(nodeIndex < nodeCount)"
	"	{"
	"		vec3 minaabb = vec3(bvh[nodeIndex].min.x, bvh[nodeIndex].min.y, bvh[nodeIndex].min.z);"
	"		vec3 maxaabb = vec3(bvh[nodeIndex].max.x, bvh[nodeIndex].max.y, bvh[nodeIndex].max.z);"
	"		int triangleIndexOffset = bvh[nodeIndex].offsetAndSize.x;"
	"		int nodeSize = bvh[nodeIndex].offsetAndSize.y;"
	"		if(RayIntersectBox(rayOrig, rayDir, minaabb, maxaabb, dist))"
	"		{"
	"			if(!(triangleIndexOffset < 0))"
	"			{"
	"				for(int i = triangleIndexOffset; i < triangleIndexOffset + nodeSize; i++)"
	"				{"
	"					vec3 a = verticies[triangles[i].a].pos.xyz;"
	"					vec3 b = verticies[triangles[i].b].pos.xyz;"
	"					vec3 c = verticies[triangles[i].c].pos.xyz;"
	"					vec4 h;"
	"					float d = RayIntersectTri(rayOrig, rayDir, a, b, c, h);"
	"					if(d < dist)"
	"					{"
	"						dist = d;"
	"						t = i;"
	"						hit = h;"
	"					}"
	"				}"
	"			}"
	"			nodeIndex++;"
	"		}"
	"		else"
	"		{"
	"			nodeIndex += triangleIndexOffset < 0 ? nodeSize : 1;"
	"		}"
	"	}"
	"	triIndex = t;"
	"}"
	
	"void RayIntersectBVHBrute(vec3 rayOrig, vec3 rayDir, out int triIndex, out vec4 hit)"
	"{"
	"	float dist = 1e10;"
	"	triIndex = -1;"

	"	for(int i = 0; i < triangleCount; i++)"
	"	{"
	"		vec3 a = verticies[triangles[i].a].pos.xyz;"
	"		vec3 b = verticies[triangles[i].b].pos.xyz;"
	"		vec3 c = verticies[triangles[i].c].pos.xyz;"
	"		vec4 h;"
	"		float d = RayIntersectTri(rayOrig, rayDir, a, b, c, h);"
	"		if(d < dist)"
	"		{"
	"			dist = d;"
	"			triIndex = i;"
	"			hit = h;"
	"		}"
	"	}"
	"}"
	
	"vec3 RandomUnitHemi(vec2 randomVal, vec3 norm)"
	"{"
	"	float a = (randomVal.x + 1.0) * PI;"
	"	float u = randomVal.y;"
	"	float u2 = u * u;"
	"	float sqrt1MinusU2 = sqrt(1.0 - u2);"
	"	float x = sqrt1MinusU2 * cos(a);"
	"	float y = sqrt1MinusU2 * sin(a);"
	"	float z = u;"
	" 	vec3 rh = vec3(x, y, z);"
	"	return rh * sign(dot(rh, norm));"
	"}"
	
	//http://amindforeverprogramming.blogspot.co.uk/2013/07/random-floats-in-glsl-330.html
	
	"uint Hash(uint x)"
	"{"
	"	x += (x << 10u);"
	"	x ^= (x >> 6u);"
	"	x += (x << 3u);"
	"	x ^= (x >> 11u);"
	"	x += (x << 15u);"
	"	return x;"
	"}"
	
	"uint Hash(uvec3 v)"
	"{"
	"	return Hash(v.x ^ Hash(v.y) ^ Hash(v.z));"
	"}"
	
	"float Random(vec2 f, float time)"
	"{"
	"	const uint mantissaMask = 0x007FFFFFu;"
	"	const uint one = 0x3F800000u;"
	"	uint h = Hash(floatBitsToUint(vec3(f, time)));"
	"	h &= mantissaMask;"
	"	h |= one;"
	"	float r2 = uintBitsToFloat(h);"
	"	return (r2 - 1.0) * 2.0 - 1.0;"
	"}"
	
	"vec2 RandomVec(vec2 f, float time0, float time1)"
	"{"
	"	return vec2(Random(f, time0), Random(f, time1));"
	"}"
	
	"layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;"
	"void main()"
	"{"
	"	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);"
	"	vec3 avColor = vec3(0, 0, 0);"
	"	vec3 rayDir = normalize(vec3(2.0f * float(gl_GlobalInvocationID.x) / 512.0f - 1.0f, 2.0f * float(gl_GlobalInvocationID.y) / 512.0f - 1.0f, -1.0f));"
	"	vec3 rayOrig = vec3(0.0f, 0.1f, 0.2f);"
	"	vec3 runningBRDF = vec3(1.0f, 1.0f, 1.0f);"
	"	int triangleIndex;"
	"	vec4 hit;"
	"	RayIntersectBVH(rayOrig, rayDir, triangleIndex, hit);"
	"	if(triangleIndex >= 0){"
	"		vec3 norma = verticies[triangles[triangleIndex].a].norm.xyz;"	
	"		vec3 normb = verticies[triangles[triangleIndex].b].norm.xyz;"
	"		vec3 normc = verticies[triangles[triangleIndex].c].norm.xyz;"
	"		vec3 norm = vec3(norma * hit.x + normb * hit.y + normc * hit.z);"
	
	"		vec3 posa = verticies[triangles[triangleIndex].a].pos.xyz;"
	"		vec3 posb = verticies[triangles[triangleIndex].b].pos.xyz;"
	"		vec3 posc = verticies[triangles[triangleIndex].c].pos.xyz;"
	"		vec3 pos = vec3(posa * hit.x + posb * hit.y + posc * hit.z);"
	
	"		vec4 lightHit;"
	"		int tirangleIndexLight;"
	"		RayIntersectBVH(pointLightPosition, normalize(pos - pointLightPosition), tirangleIndexLight, lightHit);"
	"		if(tirangleIndexLight == triangleIndex && tirangleIndexLight != -1)"
	"		{"
	"			float lightDist = length(pointLightPosition - pos);"
	"			float lightCosTheta = max(0.0f, dot(norm, normalize(pointLightPosition - pos)));"
	"			vec3 BRDFLight = 2.0f * pointLightColor * lightCosTheta * (1 / (lightDist * lightDist));"
	"			avColor = runningBRDF * BRDFLight;"
	"		}"
	"		imageStore(outputImage, storePos, vec4(avColor, 1.0f));"
	"	}"
	"	else {"
	"		imageStore(outputImage, storePos, vec4(0.0f, 0.0f, 0.0f, 1.0f));"
	"	}"
    "}"
	);
	
	Scene scene(argv[1]);
	std::cout << "Scene Loaded, verticies:" << scene.verticies.size() << " triangles:" << scene.triangles.size() << std::endl;
	scene.RebuildBVH();
	scene.AllocateGPUBuffers();

	computeShader.SetStorageBuffer<Vertex>("verticiesSB", scene.verticiesSB);
	computeShader.SetStorageBuffer<Triangle>("trianglesSB", scene.trianglesSB);
	computeShader.SetStorageBuffer<BVH::BVHNode>("bvhSB", scene.bvhSB);
	
	computeShader.SetInt("triangleCount", scene.triangles.size());
	computeShader.SetInt("nodeCount", scene.bvh.nodes.size());
	
	ShaderImage shaderImage(512, 512);
	computeShader.SetImage("outputImage", shaderImage);
	
	float randTime0 = ((float)clock())/CLOCKS_PER_SEC;
	computeShader.SetFloat("randTime0", randTime0);
	float randTime1 = ((float)clock())/CLOCKS_PER_SEC + 50.0f;
	computeShader.SetFloat("randTime1", randTime1);
	
	computeShader.SetInt("bounceCount", 2);
	computeShader.SetInt("sampleNumber", 1);  
	float pointLightPosition[] = {1.0f, 1.0f, 0.8f};
	computeShader.SetFloat3("pointLightPosition", pointLightPosition);
	float pointLightColor[] = {0.5f, 0.5f, 0.5f};
	computeShader.SetFloat3("pointLightColor", pointLightColor);
	
	computeShader.Dispatch(512 / 16, 512 / 16, 1);
	computeShader.Block();
	
	Image image(512, 512);
	shaderImage.Get(image.Data());
	
	image.ToFile("Test.png");
	std::cout << "DONE" << std::endl;
	
	GLComputeHelper::Context::Destroy();
	

	return 1;
}