#include "Scene.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>


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
 

Scene::Scene()
{
	backgroundColor[0] = 1.0f;
	backgroundColor[1] = 1.0f;
	backgroundColor[2] = 1.0f;
}
 
Scene::Scene(std::string fileName)
{
	AddModelFromFile(fileName);
	backgroundColor[0] = 1.0f;
	backgroundColor[1] = 1.0f;
	backgroundColor[2] = 1.0f;
}
 
	
bool Scene::AddModelFromFile(const std::string fileName)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	const aiScene* scn = importer.ReadFile(fileName, 
											aiProcess_CalcTangentSpace | 
											aiProcess_Triangulate |
											aiProcess_SortByPType |
											aiProcess_GenSmoothNormals | 
											aiProcess_PreTransformVertices |
											aiProcess_GenUVCoords |
											aiProcess_OptimizeMeshes | 
											aiProcess_OptimizeGraph);
	if(!scn)
	{
		std::cerr << "Scene: could load scene file" << std::endl;
		return false;
	}
	
	//get material data
	int materialNumPrev = materials.size();
	materials.resize(materialNumPrev + scn->mNumMaterials);
	for(int i = 0; i < scn->mNumMaterials; i++)
	{
		int inx = materialNumPrev + i;
		aiMaterial* material = scn->mMaterials[i];
		aiColor3D diffuseColor(1.0f, 1.0f, 1.0f);
		aiColor3D emissiveColor(0.0f, 0.0f, 0.0f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
		materials[inx].reflectance[0] = diffuseColor.r; 
		materials[inx].reflectance[1] = diffuseColor.g; 
		materials[inx].reflectance[2] = diffuseColor.b;
		materials[inx].reflectance[3] = 1.0f;		 		
		materials[inx].emission[0] = emissiveColor.r; 
		materials[inx].emission[1] = emissiveColor.g; 
		materials[inx].emission[2] = emissiveColor.b;
		materials[inx].emission[3] = 1.0f;
	}
	
	//get mesh data
	for(int i = 0; i < scn->mNumMeshes; i++)
	{
		int vNumPrev = verticies.size();
		int tNumPrev = triangles.size();
		aiMesh* mesh = scn->mMeshes[i];
		verticies.resize(vNumPrev + mesh->mNumVertices);
		for(int j = 0; j < mesh->mNumVertices; j++)
		{
			int inx = vNumPrev + j;
			verticies[inx].pos[0] = mesh->mVertices[j].x;
			verticies[inx].pos[1] = mesh->mVertices[j].y;
			verticies[inx].pos[2] = mesh->mVertices[j].z;
			verticies[inx].norm[0] = mesh->mNormals[j].x;
			verticies[inx].norm[1] = mesh->mNormals[j].y;
			verticies[inx].norm[2] = mesh->mNormals[j].z;
		}
		triangles.resize(tNumPrev + mesh->mNumFaces);
		for(int j = 0; j < mesh->mNumFaces; j++)
		{
			int inx = tNumPrev + j;
			triangles[inx].verI[0] = vNumPrev + mesh->mFaces[j].mIndices[0];
			triangles[inx].verI[1] = vNumPrev + mesh->mFaces[j].mIndices[1];
			triangles[inx].verI[2] = vNumPrev + mesh->mFaces[j].mIndices[2];
			triangles[inx].matI = materialNumPrev + mesh->mMaterialIndex;
		}
	}
	
	return true;
}

bool Scene::BackgroundCubeImageFromFile(std::string fileName[6])
{
	int width = -1;
	int height = -1; 
	for(int i = 0; i < 6; i++)
	{
		backgroundCubeImages[i].FromFile(fileName[i]);
		int w = backgroundCubeImages[i].GetWidth();
		int h = backgroundCubeImages[i].GetHeight();
		if(width != w && width != -1 || height != h && height != -1)
		{
			return false;
		}
		width = w;
		height = h;
	}
	
	backgroundCubeTexture.Allocate(width, height, GLComputeHelper::Texture::CUBE_MAP, GLComputeHelper::Texture::RGBA8, NULL, GLComputeHelper::Texture::CLAMP_TO_EDGE);
	backgroundCubeTexture.CopyCube(backgroundCubeImages[0].Data(), backgroundCubeImages[1].Data(), backgroundCubeImages[2].Data(),
								   backgroundCubeImages[3].Data(), backgroundCubeImages[4].Data(), backgroundCubeImages[5].Data());
}

void Scene::RebuildBVH(int maxdepth)
{
	bvh.Build(triangles, verticies, maxdepth);
}

void Scene::AllocateGPUBuffers()
{	
	verticiesSB.Allocate(verticies.size(), 0, &verticies[0]);
	trianglesSB.Allocate(triangles.size(), 1, &triangles[0]);
	bvhSB.Allocate(bvh.nodes.size(), 2, &bvh.nodes[0]);
	materialsSB.Allocate(materials.size(), 3, &materials[0]);
}

void Scene::AllocateGPUBufferTextures()
{
	std::vector<float> posBuffer(verticies.size() * 4);
	std::vector<float> normBuffer(verticies.size() * 4);
	float temp[] = {0.0f, 0.0f, 0.0f};
	for(int i = 0; i < verticies.size(); i++)
	{
		if(i % 3 != 0)
		{
			posBuffer[i * 4 + 0] = verticies[i].pos[0] - temp[0];
			posBuffer[i * 4 + 1] = verticies[i].pos[1] - temp[1];
			posBuffer[i * 4 + 2] = verticies[i].pos[2] - temp[2];
		}
		else
		{
			temp[0] = verticies[i].pos[0];
			temp[1] = verticies[i].pos[1];
			temp[2] = verticies[i].pos[2];
			posBuffer[i * 4 + 0] = verticies[i].pos[0];
			posBuffer[i * 4 + 1] = verticies[i].pos[1];
			posBuffer[i * 4 + 2] = verticies[i].pos[2];
		}
		
		normBuffer[i * 4 + 0] = verticies[i].norm[0];
		normBuffer[i * 4 + 1] = verticies[i].norm[1];
		normBuffer[i * 4 + 2] = verticies[i].norm[2];
	}
	std::vector<int> triBuffer(triangles.size() * 4); 
	for(int i = 0; i < triangles.size(); i++)
	{
		triBuffer[i * 4 + 0] = triangles[i].verI[0];
		triBuffer[i * 4 + 1] = triangles[i].verI[1];
		triBuffer[i * 4 + 2] = triangles[i].verI[2];
		triBuffer[i * 4 + 3] = triangles[i].matI;
	}
	
	std::vector<int> bvhBuffer(bvh.nodes.size() * 8);
	for(int i = 0; i < bvh.nodes.size(); i++)
	{
		bvhBuffer[i * 8 + 0] = *((int*)(&(bvh.nodes[i].aabb.min[0])));
		bvhBuffer[i * 8 + 1] = *((int*)(&(bvh.nodes[i].aabb.min[1])));
		bvhBuffer[i * 8 + 2] = *((int*)(&(bvh.nodes[i].aabb.min[2])));
		bvhBuffer[i * 8 + 3] = bvh.nodes[i].triangleIndexOffset;
		
		bvhBuffer[i * 8 + 4] = *((int*)(&(bvh.nodes[i].aabb.max[0])));
		bvhBuffer[i * 8 + 5] = *((int*)(&(bvh.nodes[i].aabb.max[1])));
		bvhBuffer[i * 8 + 6] = *((int*)(&(bvh.nodes[i].aabb.max[2])));
		bvhBuffer[i * 8 + 7] = bvh.nodes[i].skipIndexAndTriangleCount;
	}
	
	verticiesPosBT.Allocate(sizeof(float) * posBuffer.size(), GLComputeHelper::BufferTexture::RGBA32F, &posBuffer[0]);
	verticiesNormBT.Allocate(sizeof(float) * normBuffer.size(), GLComputeHelper::BufferTexture::RGBA32F, &normBuffer[0]);
	trianglesBT.Allocate(sizeof(int) * triBuffer.size(), GLComputeHelper::BufferTexture::RGBA32I, &triBuffer[0]);
	bvhBT.Allocate(sizeof(int) * bvhBuffer.size(), GLComputeHelper::BufferTexture::RGBA32I, &bvhBuffer[0]);
	
}

	
}//RenderEngine Namespace