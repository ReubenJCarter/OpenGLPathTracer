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
			int skipIndexAndTriangleCount; //sibling index and triangle count
			//int pad0; int pad1; 
		};
		struct BVHNode2
		{
			AABB aabb;
			int l;
			int r;
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
		std::vector<BVHNode2> nodes2;
		
		BVH();
		void Build(std::vector<Triangle>& triangleList, std::vector<Vertex>& vertexList, int maxdepth);
		void BuildNodes2();
		void ToFile(std::string fileName);
};


}//RenderEngine Namespace


#endif 