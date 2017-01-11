#ifndef SHADER_IMAGE_H
#define SHADER_IMAGE_H

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
 *Shader Image class
 *
 */
 
 
class ShaderImage
{
	public:
		enum TYPE {RGBA8, RGBA32F, RGB8, RGB32F};
		
	private:
		int width;
		int height;
		GLuint texture;
		TYPE type;
		
	public:
		ShaderImage();
		ShaderImage(int w, int h, TYPE t=RGBA32F, void* data=NULL);
		~ShaderImage();
		void Allocate(int w, int h, TYPE t=RGBA32F, void* data=NULL);
		void Deallocate();
		void Copy(void* data, int w=-1, int h=-1, int offX=0, int offY=0);
		void Get(void* data);
		unsigned int GetTexture();
		TYPE GetType();
		int GetWidth();
		int GetHeight();
};

}//GLComputeHelper namespace


#endif