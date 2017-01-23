#include "BufferTexture.h"


/**
 *
 *GLComputeHelper namespace 
 *
 */

 
namespace GLComputeHelper
{
	
	
BufferTexture::BufferTexture()
{
	glGenBuffers(1, &buffer);
	glGenTextures(1, &texture);
}

BufferTexture::~BufferTexture()
{
	glDeleteBuffers(1, &buffer);
	glDeleteBuffers(1, &texture);
}


void BufferTexture::Allocate(unsigned int s, FORMAT format, void* data)
{
	glInternalFormat = GL_RGBA32F; 
	if(format == RGBA32F) glInternalFormat = GL_RGBA32F;
	else if(format == RGB32F) glInternalFormat = GL_RGB32F;
	else if(format == RG32F) glInternalFormat = GL_RG32F;
	else if(format == R32F) glInternalFormat = GL_R32F;
	else if(format == RGBA32I) glInternalFormat = GL_RGBA32I;
	else if(format == RGB32I) glInternalFormat = GL_RGB32I;
	else if(format == RG32I) glInternalFormat = GL_RG32I;
	else if(format == R32I) glInternalFormat = GL_R32I;
	
	size = s;
	glBindBuffer(GL_TEXTURE_BUFFER, buffer);
	glBufferData(GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}


void BufferTexture::Copy(void* data)
{
	glBindBuffer(GL_TEXTURE_BUFFER, buffer);
	glBufferSubData(GL_TEXTURE_BUFFER, 0, size, data);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}


GLuint BufferTexture::GetTexture()
{
	return texture;
}

GLuint BufferTexture::GetBuffer()
{
	return buffer;
}

GLenum BufferTexture::GetInternalFormat()
{
	return glInternalFormat;
}
	
	
}