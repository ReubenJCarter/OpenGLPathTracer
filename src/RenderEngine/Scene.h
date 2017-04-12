#ifndef SCENE_H
#define SCENE_H

#include <string>

#include "DataTypes.h"
#include "Image.h"
#include "BVH.h"
#include "Camera.h"
#include "GLComputeHelper/StorageBuffer.h"
#include "GLComputeHelper/Texture.h"
#include "GLComputeHelper/BufferTexture.h"
#include "GLComputeHelper/ShaderImage.h"


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
		std::vector<std::string> imageFileNames;
		std::vector<Image> images;
		float backgroundColor[3];
		Image backgroundCubeImages[6]; 		
		
		GLComputeHelper::StorageBuffer<Vertex> verticiesSB;
		GLComputeHelper::StorageBuffer<Triangle> trianglesSB;
		GLComputeHelper::StorageBuffer<Material> materialsSB;
		GLComputeHelper::StorageBuffer<BVH::BVHNode> bvhSB;
		GLComputeHelper::BufferTexture verticiesPosBT;
		GLComputeHelper::BufferTexture verticiesNormBT;
		GLComputeHelper::BufferTexture trianglesBT;
		GLComputeHelper::BufferTexture bvhBT;
		std::vector<GLComputeHelper::ShaderImage> textures;
		GLComputeHelper::Texture backgroundCubeTexture;
	
		Scene();	
		Scene(std::string fileName);	
		bool AddModelFromFile(std::string fileName);
		bool BackgroundCubeImageFromFile(std::string fileName[6]);
		void RebuildBVH(int maxdepth=64);
		void AllocateTextures();
		void AllocateGPUBuffers();
		void AllocateGPUBufferTextures();
};


}//RenderEngine Namespace


#endif 