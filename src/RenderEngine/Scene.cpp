#include "Scene.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>
#include <algorithm>


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
	int imageNumPrev = imageFileNames.size();
	materials.resize(materialNumPrev + scn->mNumMaterials);
	for(int i = 0; i < scn->mNumMaterials; i++)
	{
		int inx = materialNumPrev + i;
		aiMaterial* material = scn->mMaterials[i];
		aiColor3D diffuseColor(1.0f, 1.0f, 1.0f);
		aiColor3D emissiveColor(0.0f, 0.0f, 0.0f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
		materials[inx].color[0] = diffuseColor.r; 
		materials[inx].color[1] = diffuseColor.g; 
		materials[inx].color[2] = diffuseColor.b;
		materials[inx].color[3] = 1.0f;		 		
		materials[inx].emission[0] = emissiveColor.r; 
		materials[inx].emission[1] = emissiveColor.g; 
		materials[inx].emission[2] = emissiveColor.b;
		materials[inx].emission[3] = 1.0f;
		
		//for all diffuse textures in material
		materials[inx].textureIndex[0] = -1;//diffuse texture
		materials[inx].textureIndex[1] = -1;
		materials[inx].textureIndex[2] = -1;
		materials[inx].textureIndex[3] = -1;
		int materialTextureNumber = material->GetTextureCount(aiTextureType_DIFFUSE);
		for(int j = 0; j < materialTextureNumber && j < 1; j++)
		{
			aiString aipath;
			if(material->GetTexture(aiTextureType_DIFFUSE, 0, &aipath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				int tinx;
				std::string texPath = aipath.C_Str();
				std::vector<std::string>::iterator it;
				it = std::find (imageFileNames.begin(), imageFileNames.end(), texPath);
				if (it != imageFileNames.end())//image already in image list 
				{
					tinx = it - imageFileNames.begin();
				}
				else //image not present in list
				{
					imageFileNames.push_back(texPath);
					tinx = imageFileNames.size() - 1;
				}
				materials[inx].textureIndex[0] = tinx;
			}
			else
			{
				
			}
		}
	}
	
	//load new textures
	images.resize(imageFileNames.size());
	for(int i = imageNumPrev; i < imageFileNames.size(); i++)
	{
		images[i].FromFile(imageFileNames[i]);
		std::cout << "Loaded Image:" << imageFileNames[i] << std::endl;
	}
	
	//get mesh data
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
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
			//verticies[inx].tang[0] = mesh->mTangents[j].x;
			//verticies[inx].tang[1] = mesh->mTangents[j].y;
			//verticies[inx].tang[2] = mesh->mTangents[j].z;
			const aiVector3D* texco;
			const aiVector3D* texco2;
			texco = (mesh->HasTextureCoords(0)) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
			texco2 = (mesh->HasTextureCoords(1)) ? &(mesh->mTextureCoords[1][j]) : &Zero3D;
			verticies[inx].uv[0] = texco->x;
			verticies[inx].uv[1] = texco->y;
			verticies[inx].uv[2] = texco2->x;
			verticies[inx].uv[3] = texco2->y;
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
	
	std::cout << "Scene: SceneLoad: Added " << verticies.size() << " verticies, " << triangles.size() << " triangles" << std::endl;
	
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
}

void Scene::RebuildBVH(int maxdepth)
{
	bvh.Build(triangles, verticies, maxdepth);
}

void Scene::AllocateTextures()
{
	backgroundCubeTexture.Allocate(backgroundCubeImages[0].GetWidth(), backgroundCubeImages[0].GetHeight(), GLComputeHelper::Texture::CUBE_MAP, GLComputeHelper::Texture::RGBA8, NULL, GLComputeHelper::Texture::CLAMP_TO_EDGE);
	backgroundCubeTexture.CopyCube(backgroundCubeImages[0].Data(), backgroundCubeImages[1].Data(), backgroundCubeImages[2].Data(),
								   backgroundCubeImages[3].Data(), backgroundCubeImages[4].Data(), backgroundCubeImages[5].Data());
								   
	textures.resize(images.size());
	for(int i = 0; i < images.size(); i++)
	{
		textures[i].Allocate(images[i].GetWidth(), images[i].GetHeight(), GLComputeHelper::ShaderImage::RGBA32F, images[i].Data());
	}
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