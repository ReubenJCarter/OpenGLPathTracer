#ifndef RENDERER_H
#define RENDERER_H


#include "Scene.h"
#include "Image.h"
#include "GLComputeHelper/ComputeShader.h"
#include "GLComputeHelper/ShaderImage.h"
#include <random>


/**
 *
 *RenderEngine Namespace
 *
 */


namespace RenderEngine
{


/**
 *
 *Renderer Class
 *
 */


class Renderer
{
	private:
		int maxBounce;
		int sampleCount;
		GLComputeHelper::ComputeShader renderShader;
		GLComputeHelper::ShaderImage shaderImage;
		std::mt19937 randGenerator; 
		std::vector<float> clearData;
		
	public:
		Renderer(int w=512, int h=512, int b=16, int s=1);
		void SetTargetSize(int w, int h);
		void SetMaxBounce(int b);
		void SetSampleCount(int s);
		void Render(Scene& scene);
		void GetImage(Image& image);
		unsigned int GetTexture();
		void ClearTarget();
};


}//RenderEngine Namespace


#endif