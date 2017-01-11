#ifndef SHADER_COMMON
#define SHADER_COMMON

#include <string>


/**
 *
 *RenderEngine Namespace
 *
 */
 

namespace RenderEngine
{
	
	
/*
 *
 *v430 Shader Src
 *
 */
 
 
const std::string v430ShaderSrc = "\n#version 430\n"; 
	
	
	
/*
 *
 *Common Types Src
 *
 */
 

const std::string commonTypesSrc = ""
"\n#define PI 3.14159265359\n"
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
//"	vec3 min;"
//"	vec3 max;"
//"	ivec3 offsetAndSize;"
"	float minX;"
"	float minY;"
"	float minZ;"
"	float maxX;"
"	float maxY;"
"	float maxZ;"
"	int offs;"
"	int sz;"
"};"

"struct Material"
"{"
"	vec4 reflectance;"
"	vec4 emission;"
"	ivec4 textureIndex;"
"};"
; 
 
	
/*
 *
 *Rand Shader Src
 *
 */
 

const std::string randShaderSrc = ""


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
"	return (r2 - 1.0) * 2.0 - 1.0;"
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
;


/*
 *
 *Ray Shader Src
 *
 */
 

const std::string rayIntersectShaderSrc = ""


/*
 *
 *RayIntersectTri Function
 *
 */

 
"bool RayIntersectTri(vec3 orig, vec3 dir, vec3 va, vec3 vb, vec3 vc, out vec4 hit, float t)"
"{"
"	vec3 p0 = va;"
"	vec3 e0 = vb - va;"
"	vec3 e1 = vc - va;"
"	vec3 pv = cross(dir, e1);"
"	float det = dot(e0, pv);"
"	vec3 tv = orig - p0;"
"	vec3 qv = cross(tv, e0);"
"	hit.y = dot(tv, pv);"
"	hit.z = dot(dir, qv);"
"	hit.x = dot(e1, qv);"
"	hit.xyz = hit.xyz / det;"
"	hit.w = hit.x;"
"	hit.x = 1.0f - hit.z - hit.y;"
"	return all(greaterThanEqual(hit, vec4(0.0001f))) && hit.w < t;"
"}"


/*
 *
 *Ray Intersect Box Function
 *
 */
 

"bool RayIntersectBox(vec3 orig, vec3 dir, vec3 bbmin, vec3 bbmax, float t)"
"{"
"	vec3 t1 = (bbmin - orig) / dir;"
"	vec3 t2 = (bbmax - orig) / dir;"
"	float tmin = max(max(min(t1.x, t2.x), min(t1.y, t2.y)), min(t1.z, t2.z));"
"	float tmax = min(min(max(t1.x, t2.x), max(t1.y, t2.y)), max(t1.z, t2.z));"
"	return tmax >= max(0.0f, tmin) && tmin < t;"
"}"
;

	
}//RenderEngine Namespace
	

#endif