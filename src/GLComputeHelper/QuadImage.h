#ifndef QUADIMAGE_H
#define QUADIMAGE_H


/**
 *
 *GLComputeHelper namespace 
 *
 */

 
namespace GLComputeHelper
{


/*
 *
 *QuadImage class
 *
 */
 
 
class QuadImage
{
	private:
		static bool inited;
		static unsigned int shaderProgram;
		
		static void Init();
		
		int width;
		int height;
		unsigned int vbo;
		unsigned int vao;
		float intensity;
		
	public:
		QuadImage(int w=512, int h=512);
		~QuadImage();
		void Draw(unsigned int glTexture);
		void SetIntensity(float i);
};


}//GLComputeHelper namespace


#endif