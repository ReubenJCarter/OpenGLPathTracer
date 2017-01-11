#ifndef IMAGE_H
#define IMAGE_H

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
 *Image Class
 *
 */

 
class Image
{
	public:
		enum TYPE {RGBA32F, RGBA8, I16};
		
	private:
		static bool inited;
		static void Init();
		
		int width; 
		int height;
		void* data;
		TYPE type; 
	
	public:
		Image();
		Image(int w, int h, TYPE t=RGBA32F);
		~Image();
		void Allocate(int w, int h, TYPE t=RGBA32F);
		void Deallocate();
		bool ToFile(std::string fileName);
		bool FromFile(std::string fileName);
		void* Data();
		int GetWidth();
		int GetHeight();
};


}//RenderEngine Namespace


#endif 