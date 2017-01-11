#include "ShaderImage.h"


/**
 *
 *GLComputeHelper namespace 
 *
 */

 
namespace GLComputeHelper
{


ShaderImage::ShaderImage()
{
	
}

ShaderImage::ShaderImage(int w, int h, TYPE t, void* data)
{
	Allocate(w, h, t, data);
}

ShaderImage::~ShaderImage()
{
	Deallocate(); 
}

void ShaderImage::Allocate(int w, int h, TYPE t, void* data)
{
	width = w;
	height = h;
	type = t;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if(type == RGBA32F)
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
	glBindTexture(GL_TEXTURE_2D, 0);
}
 
void ShaderImage::Deallocate()
{
	glDeleteTextures(1, &texture);
}

void ShaderImage::Copy(void* data, int w, int h, int offX, int offY)
{
	int wfinal = w;
	int hfinal = h;
	if(w < 0 || h < 0)
	{
		wfinal = width;
		hfinal = height;
	}
	if(type == RGBA32F)
		glTextureSubImage2D(texture, 0, offX, offY, wfinal, hfinal, GL_RGBA, GL_FLOAT, data);
}

void ShaderImage::Get(void* data)
{
	if(type == RGBA32F)
		glGetTextureImage(texture, 0, GL_RGBA, GL_FLOAT, width * height * 16, data);
}

unsigned int ShaderImage::GetTexture()
{
	return texture;
}

ShaderImage::TYPE ShaderImage::GetType()
{
	return type;
}

int ShaderImage::GetWidth()
{
	return width;
}

int ShaderImage::GetHeight()
{
	return height;
}


}//GLComputeHelper namespace