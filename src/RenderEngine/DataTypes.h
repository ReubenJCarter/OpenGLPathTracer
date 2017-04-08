#ifndef DATATYPES_H
#define DATATYPES_H


/**
 *
 *RenderEngine Namespace
 *
 */
 

namespace RenderEngine
{


/**
 *
 *AABB struct
 *
 */

 
struct AABB
{
	float min[3];
	float max[3];
};


/**
 *
 *Vertex
 *
 */

 
struct Vertex
{
	float pos[4];
	float norm[4];
	float binorm[4];
	float tang[4];
	float uv[4];
};
 

/**
 *
 *Triangle struct
 *
 */

 
struct Triangle
{
	int verI[3];
	int matI;
};


/**
 *
 *Material struct
 *
 */

 
struct Material
{
	float color[4];
	float emission[4];
	int textureIndex[4];
};


}//RenderEngine Namespace


#endif 