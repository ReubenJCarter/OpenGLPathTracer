#include "Renderer.h"

#include <time.h>  
#include <math.h>
#include <stdio.h>
#include <iostream>
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


const std::string shaderSrc = ""
"layout(rgba32f) uniform image2D outputImage;"

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

"layout (std430, binding=4) readonly  buffer materialsSB"
"{"
"	Material materials[];"
"};"

"layout (std430, binding=5) readonly buffer randNumSB"
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
"uniform vec3 backgroundColor;"
"uniform samplerCube backgroundCube;"
"uniform ivec2 targetSize;"

"uniform float cameraLength;"
"uniform vec3 cameraPosition;"
"uniform mat3 cameraRotation;"

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
"					bool triray = all(greaterThanEqual(hitTemp, vec4(0.0001f))) && hitTemp.w < dist;"

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

"layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;"
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
	clearData.resize(w * h * 4);
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
	
	renderShader.SetBufferTexture("verticiesPosBT", scene.verticiesPosBT, 5);
	renderShader.SetBufferTexture("verticiesNormBT", scene.verticiesNormBT, 6);
	renderShader.SetBufferTexture("trianglesBT", scene.trianglesBT, 7);
	renderShader.SetBufferTexture("bvhBT", scene.bvhBT, 8);
	
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
	GLComputeHelper::StorageBuffer<float> randNumSB(maxBounce * 2, 8, &randVec[0]);
	renderShader.SetStorageBuffer<float>("randNumSB", randNumSB);
	
	renderShader.SetFloat3("backgroundColor", scene.backgroundColor);
	renderShader.SetTexture("backgroundCube", scene.backgroundCubeTexture, 10);
	
	int targetSize[] = {shaderImage.GetWidth(), shaderImage.GetHeight()};
	renderShader.SetInt2("targetSize", targetSize);

	renderShader.Dispatch(targetSize[0] / 16, targetSize[1] / 16, 1);
	
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

void Renderer::ClearTarget()
{
	shaderImage.Copy(&clearData[0]);
}


}//RenderEngine Namespace