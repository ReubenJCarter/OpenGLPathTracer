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

	
}//RenderEngine Namespace