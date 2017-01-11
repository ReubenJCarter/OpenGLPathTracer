#include "BVH.h"
#include <algorithm>
#include <limits>
#include <math.h>
#include <sstream>
#include <iostream>
#include <fstream>


/**
 *
 *RenderEngine Namespace
 *
 */


namespace RenderEngine
{
	
	
/**
 *
 *BVH Class
 *
 */
 
 
int BVH::GenNodesRecurse(int parentIndex, int depth)
{
	//return if too deep this node is a leaf node
	if (depth > maxDepth)
		return 0; //return 0 as has no children or sub children

	BVHNode* parent = &nodes[parentIndex];
	
	//Find Split BB pair for parent node
	
	
	//Use SAH (we will find the best axis to split and the best splitting plane by finding the minimum of the sah cost)
	//find initial SAH for parent BB
	float lengthX = parent->aabb.max[0] - parent->aabb.min[0]; 
	float lengthY = parent->aabb.max[1] - parent->aabb.min[1]; 
	float lengthZ = parent->aabb.max[2] - parent->aabb.min[2];
	float lowestCost = parent->nodeSize * (lengthX * lengthY + lengthY * lengthZ + lengthZ * lengthX);
	float splitPlane;
	int splitAxis = -1;
	AABB splitAABB[2];
	int splitCount[2]; 
	//for each axis
	TriangleInfo* triRefs = &triangleInfo[parent->triangleIndexOffset];
	int binNumber = 1024 / depth;
	for(int axis = 0; axis < 3; axis++)
	{
		//we will bin the aabb of the parent node in an axis
		float stepSize = (parent->aabb.max[axis] - parent->aabb.min[axis]) / (float)binNumber;
		//for each bin - 1 (ie eac splitting plane)
		for(int bin = 1; bin < binNumber; bin++)
		{
			//compute the split plane
			float currentSplitPlane = parent->aabb.min[axis] + bin * stepSize;
			//compute the aabb of the two sets of triangles on each side of the split plane and store the count (num triangle on each side)
			AABB aabb[2];
			int count[2];
			count[0] = 0;
			count[1] = 0;
			aabb[0].min[0] = std::numeric_limits<float>::max();
			aabb[0].min[1] = std::numeric_limits<float>::max();
			aabb[0].min[2] = std::numeric_limits<float>::max();
			aabb[0].max[0] = std::numeric_limits<float>::min();
			aabb[0].max[1] = std::numeric_limits<float>::min();
			aabb[0].max[2] = std::numeric_limits<float>::min();
			aabb[1].min[0] = std::numeric_limits<float>::max();
			aabb[1].min[1] = std::numeric_limits<float>::max();
			aabb[1].min[2] = std::numeric_limits<float>::max();
			aabb[1].max[0] = std::numeric_limits<float>::min();
			aabb[1].max[1] = std::numeric_limits<float>::min();
			aabb[1].max[2] = std::numeric_limits<float>::min();
			for(int i = 0; i < parent->nodeSize; i++)
			{
				if(triRefs[i].centroid[axis] <= currentSplitPlane)
				{
					aabb[0].min[0] = std::min(aabb[0].min[0], triRefs[i].aabb.min[0]);
					aabb[0].min[1] = std::min(aabb[0].min[1], triRefs[i].aabb.min[1]);
					aabb[0].min[2] = std::min(aabb[0].min[2], triRefs[i].aabb.min[2]);
					aabb[0].max[0] = std::max(aabb[0].max[0], triRefs[i].aabb.max[0]);
					aabb[0].max[1] = std::max(aabb[0].max[1], triRefs[i].aabb.max[1]);
					aabb[0].max[2] = std::max(aabb[0].max[2], triRefs[i].aabb.max[2]);
					count[0]++;
				}
				else
				{
					aabb[1].min[0] = std::min(aabb[1].min[0], triRefs[i].aabb.min[0]);
					aabb[1].min[1] = std::min(aabb[1].min[1], triRefs[i].aabb.min[1]);
					aabb[1].min[2] = std::min(aabb[1].min[2], triRefs[i].aabb.min[2]);
					aabb[1].max[0] = std::max(aabb[1].max[0], triRefs[i].aabb.max[0]);
					aabb[1].max[1] = std::max(aabb[1].max[1], triRefs[i].aabb.max[1]);
					aabb[1].max[2] = std::max(aabb[1].max[2], triRefs[i].aabb.max[2]);
					count[1]++;
				}
			}
			//check for bad case with one or less triangle in 
			if (count[0] <= 1 || count[1] <= 1) continue;
			
			//compute the cost of both aabbs and sum to find total cost
			float lenX[2];
			float lenY[2];
			float lenZ[2];
			lenX[0] = aabb[0].max[0] - aabb[0].min[0];
			lenX[1] = aabb[1].max[0] - aabb[1].min[0];
			lenY[0] = aabb[0].max[1] - aabb[0].min[1];
			lenY[1] = aabb[1].max[1] - aabb[1].min[1];
			lenZ[0] = aabb[0].max[2] - aabb[0].min[2];
			lenZ[1] = aabb[1].max[2] - aabb[1].min[2];
			float cost = count[0] * (lenX[0] * lenY[0] + lenY[0] * lenZ[0] + lenZ[0] * lenX[0]) + 
						 count[1] * (lenX[1] * lenY[1] + lenY[1] * lenZ[1] + lenZ[1] * lenX[1]);
			
			//update only if the cost is lowest (we will find the minimum cost)
			if(cost < lowestCost)
			{
				lowestCost = cost;
				splitPlane = currentSplitPlane;
				splitAxis = axis;
				splitAABB[0] = aabb[0];
				splitAABB[1] = aabb[1];
				splitCount[0] = count[0];
				splitCount[1] = count[1];
			}
		}
	}
	
	//return if no split was found this node is a leaf node
	if (splitAxis == -1)
		return 0; // return 0 as node has no children
			
	//Sort List in place around split and get index of split point	
	int splitIndex = parent->nodeSize - 1;
	for(int i = 0; i < parent->nodeSize; i++)
	{
		if(i >= splitIndex)
			break; 
		
		if(triRefs[i].centroid[splitAxis] <= splitPlane)
			continue;
		
		while(triRefs[splitIndex].centroid[splitAxis] > splitPlane && i < splitIndex)
		{
			splitIndex--;
		}
		
		if(i < splitIndex)
		{
			TriangleInfo temp = triRefs[i];
			triRefs[i] = triRefs[splitIndex];
			triRefs[splitIndex] = temp;
		}
		else
		{
			break; 
		}
	}
	
	int nodeNumberBellow = 2;
	
	//Create BVHNode 0 and add to BVHNode list
	BVHNode bvhNode0;
	bvhNode0.aabb = splitAABB[0];
	bvhNode0.triangleIndexOffset = nodes[parentIndex].triangleIndexOffset;
	bvhNode0.nodeSize = splitCount[0];
	nodes.push_back(bvhNode0);
	
	//Recurse on BVHNode 0 list 
	nodeNumberBellow += GenNodesRecurse(nodes.size() - 1, depth + 1);
	
	//Create BVHNode 1 and add to list
	BVHNode bvhNode1;
	bvhNode1.aabb = splitAABB[1];
	bvhNode1.triangleIndexOffset = nodes[parentIndex].triangleIndexOffset + splitIndex;
	bvhNode1.nodeSize = splitCount[1];
	nodes.push_back(bvhNode1);
	
	//Recurse on BVHNode 1 list
	nodeNumberBellow += GenNodesRecurse(nodes.size() - 1, depth + 1);
	
	//parent is not leaf node so need to make triangleIndexOffset negative (later we will compute the actual inner node size)
	nodes[parentIndex].triangleIndexOffset = -1;
	
	//the number of nodes under the parent (is increassed by 2 from the ones created in this function + all the sub nodes created(this function also returns the number))
	nodes[parentIndex].nodeSize = nodeNumberBellow;
	
	return nodeNumberBellow;
}


BVH::BVH()
{
	
}


void BVH::Build(std::vector<Triangle>& triangleList, std::vector<Vertex>& vertexList, int maxdepth)
{
	//init things...
	maxDepth = maxdepth;
	nodes.clear();
	
	//Create list of triangles and the AABB triangle ref for each 
	triangleInfo.resize(triangleList.size());
	for(int i = 0; i < triangleList.size(); i++)
	{
		Vertex v[3];
		v[0] = vertexList[triangleList[i].verI[0]];
		v[1] = vertexList[triangleList[i].verI[1]];
		v[2] = vertexList[triangleList[i].verI[2]];
		triangleInfo[i].triangleIndex = i; 
		triangleInfo[i].aabb.min[0] = std::min(v[0].pos[0], std::min(v[1].pos[0], v[2].pos[0]));
		triangleInfo[i].aabb.min[1] = std::min(v[0].pos[1], std::min(v[1].pos[1], v[2].pos[1]));
		triangleInfo[i].aabb.min[2] = std::min(v[0].pos[2], std::min(v[1].pos[2], v[2].pos[2]));
		triangleInfo[i].aabb.max[0] = std::max(v[0].pos[0], std::max(v[1].pos[0], v[2].pos[0]));
		triangleInfo[i].aabb.max[1] = std::max(v[0].pos[1], std::max(v[1].pos[1], v[2].pos[1]));
		triangleInfo[i].aabb.max[2] = std::max(v[0].pos[2], std::max(v[1].pos[2], v[2].pos[2]));
		triangleInfo[i].centroid[0] = (triangleInfo[i].aabb.min[0] + triangleInfo[i].aabb.max[0]) / 2.0f;
		triangleInfo[i].centroid[1] = (triangleInfo[i].aabb.min[1] + triangleInfo[i].aabb.max[1]) / 2.0f;
		triangleInfo[i].centroid[2] = (triangleInfo[i].aabb.min[2] + triangleInfo[i].aabb.max[2]) / 2.0f;
	}
	
	//create parent bvh node (we wont add this to the list)
	BVHNode bvhParent;
	bvhParent.aabb.min[0] = std::numeric_limits<float>::max();
	bvhParent.aabb.min[1] = std::numeric_limits<float>::max();
	bvhParent.aabb.min[2] = std::numeric_limits<float>::max();
	bvhParent.aabb.max[0] = std::numeric_limits<float>::min();
	bvhParent.aabb.max[1] = std::numeric_limits<float>::min();
	bvhParent.aabb.max[2] = std::numeric_limits<float>::min();
	for(int i = 0; i < triangleInfo.size(); i++)
	{
		bvhParent.aabb.min[0] = std::min(bvhParent.aabb.min[0], triangleInfo[i].aabb.min[0]);
		bvhParent.aabb.min[1] = std::min(bvhParent.aabb.min[1], triangleInfo[i].aabb.min[1]);
		bvhParent.aabb.min[2] = std::min(bvhParent.aabb.min[2], triangleInfo[i].aabb.min[2]);
		bvhParent.aabb.max[0] = std::max(bvhParent.aabb.max[0], triangleInfo[i].aabb.max[0]);
		bvhParent.aabb.max[1] = std::max(bvhParent.aabb.max[1], triangleInfo[i].aabb.max[1]);
		bvhParent.aabb.max[2] = std::max(bvhParent.aabb.max[2], triangleInfo[i].aabb.max[2]);
	}
	bvhParent.triangleIndexOffset = 0;
	bvhParent.nodeSize = triangleInfo.size();
	nodes.push_back(bvhParent);
	
	//Run the recursive build function
	GenNodesRecurse(0, 1);
	
	//sort the input triangle list to match the triangle referance list
	std::vector<Triangle> newTriangleList(triangleList.size());
	for(int i = 0; i < triangleInfo.size(); i++)
		newTriangleList[i] = triangleList[triangleInfo[i].triangleIndex];	
	for(int i = 0; i < newTriangleList.size(); i++)
		triangleList[i] = newTriangleList[i];
	
	//Probably now need to sort the list of vertices to increase cache locallity see tipsify algorithm
}


void BVH::ToFile(std::string fileName)
{
	std::ofstream bvhFile(fileName.c_str());
	if (bvhFile.is_open())
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			bvhFile << nodes[i].aabb.min[0] << ", " << nodes[i].aabb.min[1] << ", " << nodes[i].aabb.min[2] << ", " 
					<< nodes[i].aabb.max[0] << ", " << nodes[i].aabb.max[1] << ", " << nodes[i].aabb.max[2] << ", "
					<< nodes[i].triangleIndexOffset << ", " << nodes[i].nodeSize << "\n";
		}
		bvhFile.close();
	}
}
 
	
}//RenderEngine Namespace