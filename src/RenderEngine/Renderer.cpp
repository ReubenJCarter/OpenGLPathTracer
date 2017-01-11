#include "Renderer.h"

#include <time.h>  
#include <math.h>
#include <stdio.h>
#include <chrono>
#include <random>
#include "ShaderCommon.h"


/**
 *
 *RenderEngine Namespace
 *
 */


namespace RenderEngine
{


/**
 *
 *Renderer Class
 *
 */
 
/*
const std::string shaderSrc = ""
"layout(rgba32f) uniform image2D outputImage;"

"layout (std430) buffer verticiesSB"
"{"
"	Vertex verticies[];"
"};"

"layout (std430) buffer trianglesSB"
"{"
"	Triangle triangles[];"
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
"uniform vec3 pointLightPosition;"
"uniform vec3 pointLightColor;"

"void RayIntersectBVH(vec3 rayOrig, vec3 rayDir, out int triIndex, out vec4 hit)"
"{"
"	float dist = 1e10;"
"	int nodeIndex = 0;"
"	triIndex = -1;"
"	while(nodeIndex < nodeCount)"
"	{"
"		vec3 minaabb = bvh[nodeIndex].min.xyz;"
"		vec3 maxaabb = bvh[nodeIndex].max.xyz;"
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
"					if(RayIntersectTri(rayOrig, rayDir, a, b, c, h, dist))"
"					{"
"						dist = h.w;"
"						triIndex = i;"
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
;*/

const std::string shaderSrc = ""
"layout(rgba32f) uniform image2D outputImage;"

"layout (std430) buffer verticiesSB"
"{"
"	Vertex verticies[];"
"};"

"layout (std430) buffer trianglesSB"
"{"
"	Triangle triangles[];"
"};"

"layout (std430) buffer bvhSB"
"{"
"	BVHNode bvh[];"
"};"

"layout (std430) buffer materialsSB"
"{"
"	Material materials[];"
"};"

"layout (std430) buffer randNumSB"
"{"
"	vec2 randNum[];"
"};"

"uniform int triangleCount;"
"uniform int nodeCount;"
"uniform int maxBounce;"
"uniform vec3 backgroundColor;"
"uniform samplerCube backgroundCube;"
"uniform ivec2 targetSize;"

"uniform float cameraLength;"
"uniform vec3 cameraPosition;"
"uniform mat3 cameraRotation;"

"void RayIntersectBVH(vec3 rayOrig, vec3 rayDir, out int triIndex, out vec4 hit)"
"{"
"	float dist = 1e10;"
"	int nodeIndex = 0;"
"	triIndex = -1;"
"	while(nodeIndex < nodeCount)"
"	{"
"		BVHNode node = bvh[nodeIndex];"
//"		vec3 minaabb = node.min.xyz;"
//"		vec3 maxaabb = node.max.xyz;"
//"		int triangleIndexOffset = node.offsetAndSize.x;"
//"		int nodeSize = node.offsetAndSize.y;"
"		vec3 minaabb = vec3(node.minX, node.minY, node.minZ);"
"		vec3 maxaabb = vec3(node.maxX, node.maxY, node.maxZ);"
"		int triangleIndexOffset = node.offs;"
"		int nodeSize = node.sz;"
"		if(RayIntersectBox(rayOrig, rayDir, minaabb, maxaabb, dist))"
"		{"
"			if(triangleIndexOffset >= 0)"
"			{"
"				for(int i = triangleIndexOffset; i < triangleIndexOffset + nodeSize; i++)"
"				{"
"					Triangle tri = triangles[i];"
"					vec3 a = verticies[tri.a].pos.xyz;"
"					vec3 b = verticies[tri.b].pos.xyz;"
"					vec3 c = verticies[tri.c].pos.xyz;"
"					vec4 h;"
"					if(RayIntersectTri(rayOrig, rayDir, a, b, c, h, dist))"
"					{"
"						dist = h.w;"
"						triIndex = i;"
"						hit = h;"
"					}"
"				}"
"			}"
"			nodeIndex++;"
"		}"
"		else"
"		{"
"			if(triangleIndexOffset >= 0)"
"			{"
"				nodeIndex++;"
"			}"
"			else"
"			{"
"				nodeIndex += nodeSize;"
"			}"
"		}"
"	}"
"}"

"void RayIntersectBrute(vec3 rayOrig, vec3 rayDir, out int triIndex, out vec4 hit)"
"{"
"	float dist = 1e10;"
"	int nodeIndex = 0;"
"	triIndex = -1;"

"	for(int i = 0; i < triangleCount; i++)"
"	{"
"		vec3 a = verticies[triangles[i].a].pos.xyz;"
"		vec3 b = verticies[triangles[i].b].pos.xyz;"
"		vec3 c = verticies[triangles[i].c].pos.xyz;"
"		vec4 h;"
"		if(RayIntersectTri(rayOrig, rayDir, a, b, c, h, dist))"
"		{"
"			dist = h.w;"
"			triIndex = i;"
"			hit = h;"
"		}"
"	}"
"}"

"layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;"
"void main()"
"{"
"	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);"
"	vec2 fragCoord = vec2(float(gl_GlobalInvocationID.x) / float(targetSize.x), float(gl_GlobalInvocationID.y) / float(targetSize.y));"

"	vec3 rayDir = cameraRotation * normalize(vec3(2.0f * fragCoord.x - 1.0f, 2.0f * fragCoord.y - 1.0f, cameraLength));"
"	vec3 rayOrig = cameraPosition;"

"	vec3 finalColor = vec3(0, 0, 0);"
"	vec3 runningBRDF = vec3(1.0f, 1.0f, 1.0f);"
"	int triangleIndex;"
"	vec4 hit;"

"	for(int i = 0; i < maxBounce; i++)"
"	{"
"		vec3 materialEmittance;"
"		vec3 materialReflectance;"
"		RayIntersectBVH(rayOrig, rayDir, triangleIndex, hit);"
"		Triangle tri; "
"		if(triangleIndex < 0)"
"		{"
"			vec4 backgroundTex = textureLod(backgroundCube, rayDir, 0);"
"			materialEmittance = backgroundTex.xyz * backgroundColor;"
"			finalColor += runningBRDF * materialEmittance.xyz;"
"			break;"
"		}"
"		else"
"		{"
"			tri = triangles[triangleIndex];"
"			Material mat = materials[tri.mat];"
"			materialEmittance = mat.emission.xyz;"
"			materialReflectance = mat.reflectance.xyz;"

"			Vertex vertA = verticies[tri.a];"
"			Vertex vertB = verticies[tri.b];"
"			Vertex vertC = verticies[tri.c];"

"			vec3 norma = vertA.norm.xyz;"	
"			vec3 normb = vertB.norm.xyz;"
"			vec3 normc = vertC.norm.xyz;"
"			vec3 norm = vec3(norma * hit.x + normb * hit.y + normc * hit.z);"

"			vec3 posa = vertA.pos.xyz;"
"			vec3 posb = vertB.pos.xyz;"
"			vec3 posc = vertC.pos.xyz;"
"			vec3 pos = vec3(posa * hit.x + posb * hit.y + posc * hit.z);"

"			vec3 newRayO = pos;"
"			vec3 newRayD = RandomUnitHemi(Random(fragCoord, randNum[i]), norm);"

"			vec3 BRDF = 2.0f * materialReflectance * max(0.0f, dot(newRayD, norm));"

"			finalColor += runningBRDF * materialEmittance;"

"			runningBRDF *= BRDF;"

"			rayOrig = newRayO;"
"			rayDir = newRayD;"
"		}"

"	}"
"	vec4 imageCurrentColor = imageLoad(outputImage, storePos);"
"	imageStore(outputImage, storePos, vec4(imageCurrentColor.xyz + finalColor, 1.0f));"
"}"
;
 

Renderer::Renderer(int w, int h, int b)
{
	
	std::string src;
	src += v430ShaderSrc;
	src += commonTypesSrc;
	src += randShaderSrc;
	src += rayIntersectShaderSrc;
	src += shaderSrc; 
	
	renderShader.Create(src);
	SetTargetSize(w, h);
	SetMaxBounce(b);
	
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	randGenerator.seed(seed);
}
 
void Renderer::SetTargetSize(int w, int h)
{
	shaderImage.Allocate(w, h);
}

void Renderer::SetMaxBounce(int b)
{
	maxBounce = b;
}

void Renderer::Render(Scene& scene)
{
	renderShader.SetStorageBuffer<Vertex>("verticiesSB", scene.verticiesSB);
	renderShader.SetStorageBuffer<Triangle>("trianglesSB", scene.trianglesSB);
	renderShader.SetStorageBuffer<BVH::BVHNode>("bvhSB", scene.bvhSB);
	renderShader.SetStorageBuffer<Material>("materialsSB", scene.materialsSB);
	renderShader.SetInt("triangleCount", scene.trianglesSB.Size());
	renderShader.SetInt("nodeCount", scene.bvhSB.Size());
	renderShader.SetInt("maxBounce", maxBounce);
	renderShader.SetImage("outputImage", shaderImage);
	
	float camPos[] = {scene.camera.position.x, scene.camera.position.y, scene.camera.position.z};
	renderShader.SetFloat3("cameraPosition", camPos);
	float camRot[9]; 
	scene.camera.GetRotation(camRot);
	renderShader.SetMat3("cameraRotation", camRot);
	float camLen = scene.camera.GetLength();
	renderShader.SetFloat("cameraLength", camLen);
	
	
	std::uniform_real_distribution<float> randDist(0.0f, 1.0f);
	std::vector<float> randVec(maxBounce * 2);
	for(int i = 0; i < maxBounce; i++)
	{
		randVec[i * 2 + 0] = randDist(randGenerator);
		randVec[i * 2 + 1] = randDist(randGenerator);
	}
	GLComputeHelper::StorageBuffer<float> randNumSB(maxBounce * 2, 5, &randVec[0]);
	renderShader.SetStorageBuffer<float>("randNumSB", randNumSB);
	
	renderShader.SetFloat3("backgroundColor", scene.backgroundColor);
	renderShader.SetTexture("backgroundCube", scene.backgroundCubeTexture, 10);
	
	int targetSize[] = {shaderImage.GetWidth(), shaderImage.GetHeight()};
	renderShader.SetInt2("targetSize", targetSize);
	
	renderShader.Dispatch(targetSize[0] / 8, targetSize[1] / 8, 1);
	renderShader.Block();
	
}

void Renderer::GetImage(Image& image)
{
	shaderImage.Get(image.Data());
}

unsigned int Renderer::GetTexture()
{
	return shaderImage.GetTexture();
}


}//RenderEngine Namespace