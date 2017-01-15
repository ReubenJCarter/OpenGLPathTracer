#include "Image.h"
#include <iostream>
#include <IL/il.h>
#include <vector> 


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
	
	
bool Image::inited = false;
	
void Image::Init()
{
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	inited = true;
}
	
Image::Image()
{
	data = NULL;
	width = 0; 
	height = 0;
	type = RGBA32F;
	
	if(!inited) Init();
}

Image::Image(int w, int h, TYPE t)
{
	Allocate(w, h, t);
	if(!inited) Init();
}

Image::~Image()
{
	Deallocate();
}

void Image::Allocate(int w, int h, TYPE t)
{
	width = w;
	height = h;
	type = t;
	if(type == RGBA8)
	{
		data = new uint8_t[w * h * 4];
	}
	else if(type == I16)
	{
		data = new uint8_t[w * h * 2];
	}
	else if(type == RGBA32F)
	{
		data = new uint8_t[w * h * 16];
	}
	else
	{
		data = new uint8_t[w * h * 4];
	}
}

void Image::Deallocate()
{
	if(width * height > 0)
	{
		delete[] (uint8_t*)data;
		width = 0;
		height = 0;
	}
}

bool Image::ToFile(std::string fileName)
{
	bool loadOK = false;
	if(width > 0 && height > 0)
	{
		ILuint ilIm;
		ilGenImages(1, &ilIm);
		ilBindImage(ilIm);
		ILboolean success = false;
		
		if(type == RGBA8)
		{
			success = ilTexImage(width, height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, data);
		}
		else if(type == I16)
		{
			success = ilTexImage(width, height, 1, 1, IL_LUMINANCE, IL_UNSIGNED_SHORT, data);
		}
		else if(type == RGBA32F)
		{
			std::vector<uint8_t> d(width * height * 4);
			for(int i = 0; i < width * height; i++)
			{
				d[i * 4 + 0] = (uint8_t)(((float*)data)[i * 4 + 0] * 255);
				d[i * 4 + 1] = (uint8_t)(((float*)data)[i * 4 + 1] * 255);
				d[i * 4 + 2] = (uint8_t)(((float*)data)[i * 4 + 2] * 255);
				d[i * 4 + 3] = (uint8_t)(((float*)data)[i * 4 + 3] * 255);
			}
			success = ilTexImage(width, height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, &d[0]);
		}
			
		if(success != true)
		{
			std::cerr << "Image:" << "failed to copy image data for saving" << std::endl;
		}
		else
		{
			ilEnable(IL_FILE_OVERWRITE);
			std::cout << "Image:Saving File:" << fileName.c_str() << std::endl;
			ilSaveImage(fileName.c_str());
			ilDisable(IL_FILE_OVERWRITE);
			loadOK = true;
		}
		ilDeleteImages(1, &ilIm);
	}
	else
	{
		std::cerr << "Image:" << "no data in image" << std::endl;
	}
	return loadOK;
}

bool Image::FromFile(std::string fileName)
{
	//make new deil image and load from file name
	ILuint ilIm;
	ilGenImages(1, &ilIm);
	ilBindImage(ilIm);
	bool loadedImageOK;
	loadedImageOK = ilLoadImage(fileName.c_str());
	if(loadedImageOK)
	{
		//get image info
		int widthIL = ilGetInteger(IL_IMAGE_WIDTH);
        int heightIL = ilGetInteger(IL_IMAGE_HEIGHT);
		
		//convert data to B4 type and format
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 
		Deallocate();
		
		//copy data
		uint8_t* dataPtr = ilGetData();
		if(dataPtr == NULL)
		{
			loadedImageOK = false;
			std::cerr << "Image:" << "No data for image:" << fileName << std::endl;
			return false;
		}
		else
		{
			Allocate(widthIL, heightIL, RGBA8);
			for(unsigned int i = 0; i < width * height * 4; i++)
			{
				((uint8_t*)data)[i] = dataPtr[i];
			}
		}
	}
	else
	{
		std::cerr << "Image:" << "Could not load image file:" << fileName << std::endl;
		return false; 
	}
	ilBindImage(0);
	ilDeleteImages(1, &ilIm);
	ilBindImage(ilIm);
	return true;
}

void* Image::Data()
{
	return data;
}

int Image::GetWidth() 
{
	return width; 
}

int Image::GetHeight() 
{
	return height;
}


}//RenderEngine Namespace