#include "Renderer.h"

#include <time.h>  
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <random>
#include <sstream>
#include "ShaderCommon.h"
#include "GLComputeHelper/Util.h"


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


const std::string shaderSrc = ""
"\n#version 430\n"

"\n#define PI 3.14159265359\n"


/*
 *Randomness
 *
 */


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

"float Random(vec2 f, float seed)"
"{"
"	const uint mantissaMask = 0x007FFFFFu;"
"	const uint one = 0x3F800000u;"
"	uint h = Hash(floatBitsToUint(vec3(f, seed)));"
"	h &= mantissaMask;"
"	h |= one;"
"	float r2 = uintBitsToFloat(h);"
"	return r2 - 1.0;"
"}"

"vec2 Random(vec2 f, vec2 seed)"
"{"
"	return vec2(Random(f, seed.x), Random(f, seed.y));"
"}"

"vec3 Random(vec2 f, vec3 seed)"
"{"
"	return vec3(Random(f, seed.x), Random(f, seed.y), Random(f, seed.z));"
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


/*
 *Definitions
 *
 */


"struct Vertex"
"{"
"	vec4 pos;"
"	vec4 norm;"
"	vec4 binorm;"
"	vec4 tang;"
"	vec4 uv;"
"};"

"struct Triangle"
"{"
"	int a;"
"	int b;"
"	int c;"
"	int mat;"
"};"

"struct BVHNode"
"{"
"	float minX;"
"	float minY;"
"	float minZ;"
"	float maxX;"
"	float maxY;"
"	float maxZ;"
"	int l;"
"	int r;"
"};"

"struct Material"
"{"
"	vec4 materialColor;"
"	vec4 emission;"
"	ivec4 textureIndex;"
"};"

"layout(rgba32f, binding=0) uniform image2D outputImage;"

"layout (std430, binding=0) readonly buffer verticiesSB"
"{"
"	Vertex verticies[];"
"};"

"layout (std430, binding=1) readonly buffer trianglesSB"
"{"
"	Triangle triangles[];"
"};"

"layout (std430, binding=2) readonly buffer bvhSB"
"{"
"	BVHNode bvh[];"
"};"

"layout (std430, binding=3) readonly  buffer materialsSB"
"{"
"	Material materials[];"
"};"

"layout (std430, binding=4) readonly buffer randNumSB"
"{"
"	vec2 randNum[];"
"};"

"uniform samplerBuffer verticiesPosBT;"
"uniform samplerBuffer verticiesNormBT;"
"uniform isamplerBuffer trianglesBT;"
"uniform isamplerBuffer bvhBT;"

"uniform int triangleCount;"
"uniform int nodeCount;"
"uniform int maxBounce;"
"uniform int sampleCount;"
"uniform vec3 backgroundColor;"
"uniform samplerCube backgroundCube;"
"uniform ivec2 targetSize;"

"uniform float cameraLength;"
"uniform vec3 cameraPosition;"
"uniform mat3 cameraRotation;"

"uniform float clearTarget;"

"uniform int textureCount;"
"layout(rgba32f, binding=1) uniform image2D textures[20];"


/*
 *Ray Intersection Tests
 *
 */


"void RayIntersectBVH(vec3 rayOrig, vec3 rayDir, out int triIndex, out vec4 hit)"
"{"
"	vec4 hitTemp;"
"	vec3 rayDirInv = vec3(1.0f, 1.0f, 1.0f) / rayDir;"
"	float dist = 1e10;"
"	triIndex = -1;"
"	int nodeIndex = 0;"
"	while(nodeIndex < nodeCount)"
"	{"
"		ivec4 n0 = texelFetch(bvhBT, nodeIndex * 2);"
"		ivec4 n1 = texelFetch(bvhBT, nodeIndex * 2 + 1);"
"		vec3 minaabb = intBitsToFloat(n0.xyz);"
"		vec3 maxaabb = intBitsToFloat(n1.xyz);"
"		int triangleIndexOffset = n0.w;"
"		int triCountAndSkip = n1.w;"

		//AABB Ray intersection 
"		vec3 t1 = (minaabb - rayOrig) * rayDirInv;"
"		vec3 t2 = (maxaabb - rayOrig) * rayDirInv;"
"		float tmin = max(max(min(t1.x, t2.x), min(t1.y, t2.y)), min(t1.z, t2.z));"
"		float tmax = min(min(max(t1.x, t2.x), max(t1.y, t2.y)), max(t1.z, t2.z));"
"		bool raybox = tmax >= max(0.0f, tmin) && tmin < dist;"

"		bool isLeaf = triangleIndexOffset >= 0;"
"		int skipInx = isLeaf ? nodeIndex + 1 : triCountAndSkip;"

"		if(raybox)"
"		{"
"			if(isLeaf)"
"			{"

"				int tLen = triangleIndexOffset + triCountAndSkip;"
"				for(int i = triangleIndexOffset; i < tLen; i++)"
"				{"
"					ivec4 tri = texelFetch(trianglesBT, i);"
"					vec3 va = texelFetch(verticiesPosBT, tri.x).xyz;"
"					vec3 e0 = texelFetch(verticiesPosBT, tri.y).xyz;"
"					vec3 e1 = texelFetch(verticiesPosBT, tri.z).xyz;"

					//Triangle Ray Intersection
"					vec3 pv = cross(rayDir, e1);"
"					float det = dot(e0, pv);"
"					vec3 tv = rayOrig - va;"
"					vec3 qv = cross(tv, e0);"
"					hitTemp.xyz = vec3(dot(e1, qv), dot(tv, pv), dot(rayDir, qv)) / det;"
"					hitTemp.w = hitTemp.x;"
"					hitTemp.x = 1.0f - hitTemp.z - hitTemp.y;"
"					bool triray = all(greaterThanEqual(hitTemp, vec4(0.0f, 0.0f, 0.0f, 0.0001f))) && hitTemp.w < dist;"

"					if(triray)"
"					{"
"						dist = hitTemp.w;"
"						triIndex = i;"
"						hit = hitTemp;"
"					}"
"				}"
"			}"
"			skipInx = nodeIndex + 1;"
"		}"
"		nodeIndex = skipInx;"
"	}"
"}"


"void RayIntersectBVHStackBased(vec3 rayOrig, vec3 rayDir, out int triIndex, out vec4 hit)"
"{"
"	vec4 hitTemp;"
"	vec3 rayDirInv = vec3(1.0f, 1.0f, 1.0f) / rayDir;"
"	float dist = 1e10;"
"	triIndex = -1;"
"	int nodeIndex = 0;"
"	int stack[128];"
"	int stackPtr = 0;"
"	stack[0] = 0;"

	//while stack not empty
"	while(stackPtr >= 0)"
"	{"
		//pop node from stack
"		nodeIndex = stack[stackPtr--];"

		//get node data
"		ivec4 n0 = texelFetch(bvhBT, nodeIndex * 2);"
"		ivec4 n1 = texelFetch(bvhBT, nodeIndex * 2 + 1);"
"		vec3 minaabb = intBitsToFloat(n0.xyz);"
"		vec3 maxaabb = intBitsToFloat(n1.xyz);"
"		int triangleIndexOffset = n0.w;"
"		int triCountAndSkip = n1.w;"

"		bool isLeaf = triangleIndexOffset >= 0;"

"		if(!isLeaf)"
"		{"

			//AABB Ray intersection 
"			vec3 t1 = (minaabb - rayOrig) * rayDirInv;"
"			vec3 t2 = (maxaabb - rayOrig) * rayDirInv;"
"			float tmin = max(max(min(t1.x, t2.x), min(t1.y, t2.y)), min(t1.z, t2.z));"
"			float tmax = min(min(max(t1.x, t2.x), max(t1.y, t2.y)), max(t1.z, t2.z));"
"			bool raybox = tmax >= max(0.0f, tmin) && tmin < dist;"

"			if(raybox)"
"			{"
"				stack[++stackPtr] = -triangleIndexOffset;" //add right child to stack
"				stack[++stackPtr] = nodeIndex + 1;" //add left child to stack
"			}"
"		}"
"		else"
"		{"

"			int tLen = triangleIndexOffset + triCountAndSkip;"
"			for(int i = triangleIndexOffset; i < tLen; i++)"
"			{"
"				ivec4 tri = texelFetch(trianglesBT, i);"
"				vec3 va = texelFetch(verticiesPosBT, tri.x).xyz;"
"				vec3 e0 = texelFetch(verticiesPosBT, tri.y).xyz;"
"				vec3 e1 = texelFetch(verticiesPosBT, tri.z).xyz;"

				//Triangle Ray Intersection
"				vec3 pv = cross(rayDir, e1);"
"				float det = dot(e0, pv);"
"				vec3 tv = rayOrig - va;"
"				vec3 qv = cross(tv, e0);"
"				hitTemp.xyz = vec3(dot(e1, qv), dot(tv, pv), dot(rayDir, qv)) / det;"
"				hitTemp.w = hitTemp.x;"
"				hitTemp.x = 1.0f - hitTemp.z - hitTemp.y;"
"				bool triray = all(greaterThanEqual(hitTemp, vec4(0.0f, 0.0f, 0.0f, 0.0001f))) && hitTemp.w < dist;"

"				if(triray)"
"				{"
"					dist = hitTemp.w;"
"					triIndex = i;"
"					hit = hitTemp;"
"				}"
"			}"
"		}"
"	}"
"}"


"void RayIntersectBVHExperemental(vec3 rayOrig, vec3 rayDir, out int triIndex, out vec4 hit)"
"{"
"	vec4 hitTemp;"
"	vec3 rayDirInv = vec3(1.0f, 1.0f, 1.0f) / rayDir;"
"	float dist = 1e10;"
"	triIndex = -1;"
"	int nodeIndex = 0;"
"	for(nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)"
"	{"
"		ivec4 n0 = texelFetch(bvhBT, nodeIndex * 2);"
"		ivec4 n1 = texelFetch(bvhBT, nodeIndex * 2 + 1);"
"		vec3 minaabb = intBitsToFloat(n0.xyz);"
"		vec3 maxaabb = intBitsToFloat(n1.xyz);"
"		int triangleIndexOffset = n0.w;"
"		int triCountAndSkip = n1.w;"

		//AABB Ray intersection 
"		vec3 t1 = (minaabb - rayOrig) * rayDirInv;"
"		vec3 t2 = (maxaabb - rayOrig) * rayDirInv;"
"		float tmin = max(max(min(t1.x, t2.x), min(t1.y, t2.y)), min(t1.z, t2.z));"
"		float tmax = min(min(max(t1.x, t2.x), max(t1.y, t2.y)), max(t1.z, t2.z));"
"		bool raybox = tmax >= max(0.0f, tmin) && tmin < dist;"

"		bool isLeaf = triangleIndexOffset >= 0;"
"		int skipInx = isLeaf ? nodeIndex + 1 : triCountAndSkip;"

"		if(raybox)"
"		{"
"			if(isLeaf)"
"			{"

"				int tLen = triangleIndexOffset + triCountAndSkip;"
"				for(int i = triangleIndexOffset; i < tLen; i++)"
"				{"
"					ivec4 tri = texelFetch(trianglesBT, i);"
"					vec3 va = texelFetch(verticiesPosBT, tri.x).xyz;"
"					vec3 e0 = texelFetch(verticiesPosBT, tri.y).xyz;"
"					vec3 e1 = texelFetch(verticiesPosBT, tri.z).xyz;"

					//Triangle Ray Intersection
"					vec3 pv = cross(rayDir, e1);"
"					float det = dot(e0, pv);"
"					vec3 tv = rayOrig - va;"
"					vec3 qv = cross(tv, e0);"
"					hitTemp.xyz = vec3(dot(e1, qv), dot(tv, pv), dot(rayDir, qv)) / det;"
"					hitTemp.w = hitTemp.x;"
"					hitTemp.x = 1.0f - hitTemp.z - hitTemp.y;"
"					bool triray = all(greaterThanEqual(hitTemp, vec4(0.0f, 0.0f, 0.0f, 0.0001f))) && hitTemp.w < dist;"

"					if(triray)"
"					{"
"						dist = hitTemp.w;"
"						triIndex = i;"
"						hit = hitTemp;"
"					}"
"				}"
"			}"
"		}"
"		else"
"		{"
"			nodeIndex = skipInx - 1;"
"		}"
"	}"
"}"


"void RayIntersectBrute(vec3 rayOrig, vec3 rayDir, out int triIndex, out vec4 hit)"
"{"
"	float dist = 1e10;"
"	int nodeIndex = 0;"
"	triIndex = -1;"
"	vec4 hitTemp;"

"	for(int i = 0; i < triangleCount; i++)"
"	{"
"		ivec4 tri = texelFetch(trianglesBT, i);"
"		vec3 va = texelFetch(verticiesPosBT, tri.x).xyz;"
"		vec3 e0 = texelFetch(verticiesPosBT, tri.y).xyz;"
"		vec3 e1 = texelFetch(verticiesPosBT, tri.z).xyz;"
"		vec4 h;"

		//Triangle Ray Intersection
"		vec3 pv = cross(rayDir, e1);"
"		float det = dot(e0, pv);"
"		vec3 tv = rayOrig - va;"
"		vec3 qv = cross(tv, e0);"
"		hitTemp.xyz = vec3(dot(e1, qv), dot(tv, pv), dot(rayDir, qv)) / det;"
"		hitTemp.w = hitTemp.x;"
"		hitTemp.x = 1.0f - hitTemp.z - hitTemp.y;"
"		bool triray = all(greaterThanEqual(hitTemp, vec4(0.0f, 0.0f, 0.0f, 0.0001f))) && hitTemp.w < dist;"

"		if(triray)"
"		{"
"			dist = h.w;"
"			triIndex = i;"
"			hit = h;"
"		}"
"	}"
"}"


/*
 *Cook Torrence 
 *
 */


"vec3 lerp(vec3 a, vec3 b, float w)"
"{"
"  return a + w * (b - a);"
"}"
 
"float saturate(float v)"
"{"
"	return clamp(v, 0.0f, 1.0f);"
"}"

"float chiGGX(float v)"
"{"
"    return v > 0.0f ? 1.0f : 0.0f;"
"}"

"float GGX_Distribution(vec3 n, vec3 h, float alpha)"
"{"
"    float NoH = dot(n,h);"
"    float alpha2 = alpha * alpha;"
"    float NoH2 = NoH * NoH;"
"    float den = NoH2 * alpha2 + (1 - NoH2);"
"    return (chiGGX(NoH) * alpha2) / ( PI * den * den );"
"}"

"float GGX_PartialGeometryTerm(vec3 v, vec3 n, vec3 h, float alpha)"
"{"
"   float VoH2 = clamp(dot(v,h), 0.0f, 1.0f);"
"	float chi = chiGGX(VoH2 / clamp(dot(v,n), 0.0f, 1.0f));"
"   VoH2 = VoH2 * VoH2;"
"   float tan2 = ( 1 - VoH2 ) / VoH2;"
"   return (chi * 2) / (1 + sqrt( 1 + alpha * alpha * tan2));"
"}"

"vec3 Fresnel_Schlick(vec3 halfVector, vec3 viewVector, vec3 F0)"
"{"
"  return F0 + (1.0f - F0) * pow( 1.0f - saturate(dot( halfVector, viewVector )), 5);"
"}"

"vec3 GGX_Specular(vec3 normal, vec3 viewVector, vec3 lightVector, float roughness, vec3 F0, out vec3 kS)"
"{"
"    float  NoV = saturate(dot(normal, viewVector));"
"    float  NoL = saturate(dot(normal, lightVector));"

	// Calculate the half vector
"	vec3 halfVector = normalize(lightVector + viewVector);"
"	float cosT = saturate(dot( lightVector, normal ));"
	
	// Calculate fresnel
"	vec3 fresnel = Fresnel_Schlick(halfVector, viewVector, F0 );"
	
	// Geometry term
"	float geometry = GGX_PartialGeometryTerm(viewVector, normal, halfVector, roughness) * GGX_PartialGeometryTerm(lightVector, normal, halfVector, roughness);"

	//Distribution term
"	float distribution = GGX_Distribution(normal, halfVector, roughness);"
	
	// Accumulate the radiance
"	float denominator = saturate( 4.0f * ( NoV * NoL + 0.05f) );"
"	vec3 radiance = geometry * fresnel * distribution * cosT / denominator;"

    // final return values
"	kS = fresnel;"
"   kS = clamp(kS, vec3(0.0f), vec3(1.0f));"
"   return radiance;"
"}"


/*
 *main
 *
 */


"layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;"
"void main()"
"{"
"	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);"
"	vec2 fragCoord = vec2(float(gl_GlobalInvocationID.x) / float(targetSize.x), float(gl_GlobalInvocationID.y) / float(targetSize.y));"
"	vec3 sampleColor = vec3(0, 0, 0);"

"	for(int s = 0; s < sampleCount; s++)"
"	{"

"	vec3 finalColor = vec3(0, 0, 0);"
"	vec3 rayDir = cameraRotation * normalize(vec3( (float(targetSize.x) / float(targetSize.y)) * (2.0f * fragCoord.x - 1.0f), 2.0f * fragCoord.y - 1.0f, cameraLength));"
"	vec3 rayOrig = cameraPosition;"

"	vec3 runningReflectanceFactor = vec3(1.0f, 1.0f, 1.0f);"
"	int triangleIndex;"
"	vec4 hit;"

"	for(int i = 0; i < maxBounce; i++)"
"	{"
"		RayIntersectBVHStackBased(rayOrig, rayDir, triangleIndex, hit);"
"		Triangle tri; "
"		if(triangleIndex < 0)"
"		{"
"			vec4 backgroundTex = textureLod(backgroundCube, rayDir, 0);"
"			vec3 materialEmittance = backgroundTex.xyz * backgroundColor;"
"			finalColor += runningReflectanceFactor * materialEmittance;"
"			break;"
"		}"
"		else"
"		{"
"			tri = triangles[triangleIndex];"
"			Material mat = materials[tri.mat];"

"			vec3 materialEmittance = mat.emission.xyz;"
"			vec3 materialColor = mat.materialColor.xyz;"
"			float materialRoughness = 0.2f;"
"			float materialIOR = 1.250f;"
"			float materialMetallic = 0.001f;"

"			Vertex vertA = verticies[tri.a];"
"			Vertex vertB = verticies[tri.b];"
"			Vertex vertC = verticies[tri.c];"
"			vec3 norma = vertA.norm.xyz;"	
"			vec3 normb = vertB.norm.xyz;"
"			vec3 normc = vertC.norm.xyz;"
"			vec3 norm = normalize(vec3(norma * hit.x + normb * hit.y + normc * hit.z));"
"			vec3 posa = vertA.pos.xyz;"
"			vec3 posb = vertB.pos.xyz;"
"			vec3 posc = vertC.pos.xyz;"
"			vec3 pos = vec3(posa * hit.x + posb * hit.y + posc * hit.z);"
"			vec2 uv0a = vertA.uv.xy;"
"			vec2 uv0b = vertB.uv.xy;"
"			vec2 uv0c = vertC.uv.xy;"
"			vec2 uv0 = vec2(uv0a * hit.x + uv0b * hit.y + uv0c * hit.z);"

"			if(mat.textureIndex.x > -1)"
"			{"
"				ivec2 imgSize = imageSize(textures[mat.textureIndex.x]);"
"				ivec2 pixelCoord = ivec2(uv0.x * imgSize.x, uv0.y * imgSize.y);"
"				materialColor *= imageLoad(textures[mat.textureIndex.x], pixelCoord).xyz;"
"			}"

"			vec3 newRayO = pos;"
"			vec3 newRayD = RandomUnitHemi(Random(vec2(gl_GlobalInvocationID.xy), randNum[i + maxBounce * s]) * 2.0f - vec2(1.0f, 1.0f), norm);"

"			materialRoughness = saturate(materialRoughness);"
"			vec3 F0 = vec3(abs ((1.0f - materialIOR) / (1.0f + materialIOR)));"
"			F0 = F0 * F0;"
"			F0 = lerp(F0, materialColor, materialMetallic);"

"			vec3 ks = vec3(0.0f);"
"			vec3 specular = GGX_Specular(norm, -rayDir, newRayD, materialRoughness, F0, ks );"
"			vec3 kd = (1.0f - ks) * (1.0f - materialMetallic);"

"			vec3 reflectanceFactor = ((kd * materialColor / PI * max(0.0f, dot(newRayD, norm))) + specular) / (1.0f / (2.0f * PI));"

"			finalColor += runningReflectanceFactor * materialEmittance;"

"			runningReflectanceFactor *= reflectanceFactor;"

"			rayOrig = newRayO;"
"			rayDir = newRayD;"
"		}"

"	}"
"	sampleColor += finalColor;"
"	}"
"	vec4 imageCurrentColor = imageLoad(outputImage, storePos) * clearTarget;"
"	imageStore(outputImage, storePos, vec4(imageCurrentColor.xyz + sampleColor / float(sampleCount), 1.0f));"
"}"
;
 

Renderer::Renderer(int w, int h, int b, int s)
{
	std::string src;
	src += shaderSrc; 
	
	renderShader.Create(src);
	SetTargetSize(w, h);
	SetMaxBounce(b);
	SetSampleCount(s);
	
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	randGenerator.seed(seed);
	
	clearTarget = true;
	clearFactor = 0.0f;
}
 
void Renderer::SetTargetSize(int w, int h)
{
	shaderImage.Allocate(w, h);
}

void Renderer::SetMaxBounce(int b)
{
	maxBounce = b;
}

void Renderer::SetSampleCount(int s)
{
	sampleCount = s;
}

void Renderer::Render(Scene& scene)
{
	renderShader.SetStorageBuffer<Vertex>("verticiesSB", scene.verticiesSB);
	renderShader.SetStorageBuffer<Triangle>("trianglesSB", scene.trianglesSB);
	renderShader.SetStorageBuffer<BVH::BVHNode>("bvhSB", scene.bvhSB);
	renderShader.SetStorageBuffer<Material>("materialsSB", scene.materialsSB);
	
	renderShader.SetBufferTexture("verticiesPosBT", scene.verticiesPosBT, 0);
	renderShader.SetBufferTexture("verticiesNormBT", scene.verticiesNormBT, 1);
	renderShader.SetBufferTexture("trianglesBT", scene.trianglesBT, 2);
	renderShader.SetBufferTexture("bvhBT", scene.bvhBT, 3);
	
	renderShader.SetInt("triangleCount", scene.trianglesSB.Size());
	renderShader.SetInt("nodeCount", scene.bvhSB.Size());
	renderShader.SetInt("maxBounce", maxBounce);
	renderShader.SetInt("sampleCount", sampleCount); 
	renderShader.SetImage("outputImage", shaderImage);
	
	if(clearTarget)
	{
		
		renderShader.SetFloat("clearTarget", clearFactor);	
	}
	else
	{
		renderShader.SetFloat("clearTarget", 1.0f);
	}
	
	renderShader.SetInt("textureCount", scene.textures.size());
	for(int i = 0; i < scene.textures.size(); i++)
	{
		std::stringstream ss;
		ss << "textures[" << i << "]";
		renderShader.SetImage(ss.str(), scene.textures[i]);
		//renderShader.SetImage("diffuseTex", scene.textures[i]);
	}
	
	float camPos[] = {scene.camera.position.x, scene.camera.position.y, scene.camera.position.z};
	renderShader.SetFloat3("cameraPosition", camPos);
	float camRot[9]; 
	scene.camera.GetRotation(camRot);
	renderShader.SetMat3("cameraRotation", camRot);
	float camLen = scene.camera.GetLength();
	renderShader.SetFloat("cameraLength", camLen);
	
	std::uniform_real_distribution<float> randDist(0.0f, 1.0f);
	std::vector<float> randVec(maxBounce * sampleCount * 2);
	for(int i = 0; i < maxBounce * sampleCount; i++)
	{
		randVec[i * 2 + 0] = randDist(randGenerator);
		randVec[i * 2 + 1] = randDist(randGenerator);
	}
	
	GLComputeHelper::StorageBuffer<float> randNumSB(maxBounce * sampleCount * 2, 4, &randVec[0]); 
	renderShader.SetStorageBuffer<float>("randNumSB", randNumSB);
	
	renderShader.SetFloat3("backgroundColor", scene.backgroundColor);
	renderShader.SetTexture("backgroundCube", scene.backgroundCubeTexture, 4);
	
	int targetSize[] = {shaderImage.GetWidth(), shaderImage.GetHeight()};
	renderShader.SetInt2("targetSize", targetSize);

	renderShader.Dispatch(targetSize[0] / 16, targetSize[1] / 16, 1);
	
	renderShader.Block();
	
	clearTarget = false;
}

void Renderer::GetImage(Image& image)
{
	shaderImage.Get(image.Data());
}

unsigned int Renderer::GetTexture()
{
	return shaderImage.GetTexture();
}

void Renderer::ClearTarget(float factor)
{
	clearFactor = factor;
	clearTarget = true;
}


}//RenderEngine Namespace