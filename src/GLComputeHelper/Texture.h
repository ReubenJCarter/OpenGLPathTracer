#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>


/**
 *
 *GLComputeHelper namespace 
 *
 */

 
namespace GLComputeHelper
{
	
	
/**
 *
 *Texture
 *
 */
 
 
class Texture
{
	public:
		enum FORMAT {RGBA8, RGBA32F, RGB8, RGB32F};
		enum WRAPPING {REPEAT, REPEAT_MIRROR, CLAMP_TO_EDGE};
		enum FILTER {LINEAR, NEAREST, LINEAR_MIPMAP_LINEAR, NEAREST_MIPMAP_LINEAR, LINEAR_MIPMAP_NEAREST, NEAREST_MIPMAP_NEAREST};
		enum TYPE{TEXTURE_2D, CUBE_MAP};
		
	private:
		int width;
		int height;
		GLuint texture;
		TYPE type; 
		FORMAT format;
		WRAPPING wrapping;
		FILTER minFilter;
		FILTER magFilter;
		float anisotropy;
		
		int glType;
		int glMinFilter;
		int glMagFilter;
		int glWrapping;
		int glInterrnalFormat;
		int glDataFormat;
		int glDataType;
		
	public:
		Texture();
		Texture(int w, int h, TYPE t=TEXTURE_2D, FORMAT f=RGBA32F, void* data=NULL, WRAPPING wrap=REPEAT, FILTER min=LINEAR, FILTER mag=LINEAR, float aniso=16.0f);
		~Texture();
		void Allocate(int w, int h, TYPE t=TEXTURE_2D, FORMAT f=RGBA32F, void* data=NULL, WRAPPING wrap=REPEAT, FILTER min=LINEAR, FILTER mag=LINEAR, float aniso=16.0f);
		void Deallocate();
		void Copy(void* data, int w=-1, int h=-1, int offX=0, int offY=0);
		void CopyCube(void* dataXPos, void* dataXNeg, void* dataYPos, void* dataYNeg, void* dataZPos, void* dataZNeg);
		void Get(void* data);
		void GenerateMipMap();
		unsigned int GetTexture();
		FORMAT GetFormat();
		TYPE GetType();
		int GetWidth();
		int GetHeight();
};

}//GLComputeHelper namespace


#endif