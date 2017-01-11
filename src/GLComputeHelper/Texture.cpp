#include "Texture.h"


/**
 *
 *GLComputeHelper namespace 
 *
 */

 
namespace GLComputeHelper
{


Texture::Texture()
{
	texture = -1;
}

Texture::Texture(int w, int h, TYPE t, FORMAT f, void* data, WRAPPING wrap, FILTER min, FILTER mag, float aniso)
{
	Allocate(w, h, t, f, data, wrap, min, mag, aniso);
}

Texture::~Texture()
{
	Deallocate(); 
}

void Texture::Allocate(int w, int h, TYPE t, FORMAT f, void* data, WRAPPING wrap, FILTER min, FILTER mag, float aniso)
{
	//set internal variables
	width = w;
	height = h;
	type = t;
	format = f;
	wrapping = wrap;
	minFilter= min;
	magFilter = mag;
	anisotropy = aniso;
	
	//set gl variables
	glType = GL_TEXTURE_2D;
	if(type == TEXTURE_2D) glType = GL_TEXTURE_2D;
	else if(type == TEXTURE_BUFFER) glType = GL_TEXTURE_BUFFER;
	else if(type == CUBE_MAP) glType = GL_TEXTURE_CUBE_MAP;
	
	glMinFilter = GL_LINEAR;
	if(min == LINEAR){ glMinFilter = GL_LINEAR;}
	else if(min == NEAREST){ glMinFilter = GL_NEAREST;}
	else if(min == LINEAR_MIPMAP_LINEAR){ glMinFilter = GL_LINEAR_MIPMAP_LINEAR;}
	else if(min == NEAREST_MIPMAP_LINEAR){ glMinFilter = GL_NEAREST_MIPMAP_LINEAR;}
	else if(min == LINEAR_MIPMAP_NEAREST){ glMinFilter = GL_LINEAR_MIPMAP_NEAREST;}
	else if(min == NEAREST_MIPMAP_NEAREST){ glMinFilter = GL_NEAREST_MIPMAP_NEAREST;}
	
	glMagFilter = GL_LINEAR;
	if(mag == LINEAR){ glMagFilter = GL_LINEAR;}
	else if(mag == NEAREST){ glMagFilter = GL_NEAREST;}
	else if(mag == LINEAR_MIPMAP_LINEAR){ glMagFilter = GL_LINEAR_MIPMAP_LINEAR;}
	else if(mag == NEAREST_MIPMAP_LINEAR){ glMagFilter = GL_NEAREST_MIPMAP_LINEAR;}
	else if(mag == LINEAR_MIPMAP_NEAREST){ glMagFilter = GL_LINEAR_MIPMAP_NEAREST;}
	else if(mag == NEAREST_MIPMAP_NEAREST){ glMagFilter = GL_NEAREST_MIPMAP_NEAREST;}
	
	glWrapping = GL_REPEAT;
	if(wrapping == REPEAT) glWrapping = GL_REPEAT;
	else if(wrapping == REPEAT_MIRROR) glWrapping = GL_MIRRORED_REPEAT;
	else if(wrapping == CLAMP_TO_EDGE) glWrapping = GL_CLAMP_TO_EDGE;
	
	glInterrnalFormat = GL_RGBA32F;
	glDataFormat = GL_RGBA;
	glDataType = GL_FLOAT;
	if(format == RGBA32F)
	{
		glInterrnalFormat = GL_RGBA32F;
		glDataFormat = GL_RGBA;
		glDataType = GL_FLOAT;
	}
	else if(format == RGBA8)
	{
		glInterrnalFormat = GL_RGBA8;
		glDataFormat = GL_RGBA;
		glDataType = GL_UNSIGNED_BYTE;
	}
	
	//create texture
	glGenTextures(1, &texture);
	glBindTexture(glType, texture);
	
	//allocate texture
	if(glType == GL_TEXTURE_CUBE_MAP)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, glInterrnalFormat, width, height, 0, glDataFormat, glDataType, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, glInterrnalFormat, width, height, 0, glDataFormat, glDataType, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, glInterrnalFormat, width, height, 0, glDataFormat, glDataType, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, glInterrnalFormat, width, height, 0, glDataFormat, glDataType, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, glInterrnalFormat, width, height, 0, glDataFormat, glDataType, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, glInterrnalFormat, width, height, 0, glDataFormat, glDataType, NULL);
	}
	else
	{
		glTexImage2D(glType, 0, glInterrnalFormat, width, height, 0, glDataFormat, glDataType, data);
	}
	
	//set texture params
	glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, glMinFilter);
	glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, glMagFilter);
	glTexParameteri(glType, GL_TEXTURE_WRAP_S, glWrapping);
	glTexParameteri(glType, GL_TEXTURE_WRAP_T, glWrapping);
	
	//atempt to set anisotropy
	if(GLEW_EXT_texture_filter_anisotropic && anisotropy > 0.00001f)
	{
		float maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		if(anisotropy <= maxAnisotropy)
		{
			glTexParameterf(glType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
		}
		else
		{
			glTexParameterf(glType, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
		}
	}
	
	glBindTexture(glType, 0);
}
 
void Texture::Deallocate()
{
	glDeleteTextures(1, &texture);
}

void Texture::Copy(void* data, int w, int h, int offX, int offY)
{
	int wfinal = w;
	int hfinal = h;
	if(w < 0 || h < 0)
	{
		wfinal = width;
		hfinal = height;
	}
	glTexSubImage2D(texture, 0, offX, offY, wfinal, hfinal, glDataFormat, glDataType, data);
}

void Texture::CopyCube(void* dataXPos, void* dataXNeg, void* dataYPos, void* dataYNeg, void* dataZPos, void* dataZNeg)
{
	glBindTexture(glType, texture);
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, width, height, glDataFormat, glDataType, dataXPos);
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, width, height, glDataFormat, glDataType, dataXNeg);
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, width, height, glDataFormat, glDataType, dataYPos);
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, width, height, glDataFormat, glDataType, dataYNeg);
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, width, height, glDataFormat, glDataType, dataZPos);
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, width, height, glDataFormat, glDataType, dataZNeg);
	glBindTexture(glType, 0);
}

void Texture::Get(void* data)
{
	glGetTextureImage(texture, 0, glDataFormat, glDataType, width * height * 16, data);
}

void Texture::GenerateMipMap()
{
	glBindTexture(glType, texture);
	glGenerateMipmap(glType);
	glBindTexture(glType, 0);
}

unsigned int Texture::GetTexture()
{
	return texture;
}

Texture::FORMAT Texture::GetFormat()
{
	return format;
}

Texture::TYPE Texture::GetType()
{
	return type;
}

int Texture::GetWidth()
{
	return width;
}

int Texture::GetHeight()
{
	return height;
}


}//GLComputeHelper namespace