#ifndef BVH_H
#define BVH_H

#include "DataTypes.h"
#include <vector>
#include <string>


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

 
class BVH
{
	public:
		struct BVHNode //exactly 32 bytes (one cache line...) not any more....
		{
			AABB aabb;
			int triangleIndexOffset; //offset into triangle Index (negative if not leaf node)
			int nodeSize; //total number of children under it (including sub children etc.), also the triangle number for leaf nodes. 
			//int pad0; int pad1; 
		};
		
	private:
		struct TriangleInfo
		{
			int triangleIndex;
			AABB aabb; 
			float centroid[3];
		};
		
		std::vector<TriangleInfo> triangleInfo;
		int maxDepth;
	
		int GenNodesRecurse(int parentIndex, int depth);
		
	public:
		std::vector<BVHNode> nodes;
		
		BVH();
		void Build(std::vector<Triangle>& triangleList, std::vector<Vertex>& vertexList, int maxdepth);
		void ToFile(std::string fileName);
};


}//RenderEngine Namespace


#endif 