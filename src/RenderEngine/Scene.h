#ifndef SCENE_H
#define SCENE_H

#include <string>

#include "DataTypes.h"
#include "Image.h"
#include "BVH.h"
#include "Camera.h"
#include "GLComputeHelper/StorageBuffer.h"
#include "GLComputeHelper/Texture.h"

/**
 *
 *RenderEngine Namespace
 *
 */
 

namespace RenderEngine
{


/**
 *
 *Scene Class
 *
 */

 
class Scene
{
	public:
		Camera camera;
		BVH bvh;
		std::vector<Vertex> verticies;
		std::vector<Triangle> triangles;
		std::vector<Material> materials;
		std::vector<Image> images;
		GLComputeHelper::StorageBuffer<Vertex> verticiesSB;
		GLComputeHelper::StorageBuffer<Triangle> trianglesSB;
		GLComputeHelper::StorageBuffer<Material> materialsSB;
		GLComputeHelper::StorageBuffer<BVH::BVHNode> bvhSB;
		std::vector<GLComputeHelper::Texture> textures;
		float backgroundColor[3];
		Image backgroundCubeImages[6]; 
		GLComputeHelper::Texture backgroundCubeTexture;
	
		Scene();	
		Scene(std::string fileName);	
		bool AddModelFromFile(std::string fileName);
		bool BackgroundCubeImageFromFile(std::string fileName[6]);
		void RebuildBVH(int maxdepth=64);
		void AllocateGPUBuffers();
};


}//RenderEngine Namespace


#endif 