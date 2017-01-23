#ifndef BUFFER_TEXTURE_H
#define BUFFER_TEXTURE_H

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
 *BufferTexture
 *
 */
 
 
class BufferTexture
{
	public:
		enum FORMAT {RGBA32F, RGB32F, RG32F, R32F, RGBA32I, RGB32I, RG32I, R32I};
		
	private:
		int size;
		GLenum glInternalFormat;
		GLuint texture;
		GLuint buffer;
		
	public:
		BufferTexture();
		~BufferTexture();
		void Allocate(unsigned int s, FORMAT format, void* data=NULL);
		void Copy(void* data);
		GLuint GetTexture();
		GLuint GetBuffer();
		GLenum GetInternalFormat();
};


}//GLComputeHelper namespace


#endif