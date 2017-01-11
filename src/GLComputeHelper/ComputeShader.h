#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include "StorageBuffer.h"
#include "ShaderImage.h"
#include "Texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>


/**
 *
 *GLComputeHelper namespace 
 *
 */
 

namespace GLComputeHelper
{
	
	
/**
 *
 *ComputeShader Class
 *
 */

	
class ComputeShader
{
	private:
		GLuint computeShader;
		GLuint computeProgram;
		
	public:
		ComputeShader();
		ComputeShader(std::string shaderSrc);
		~ComputeShader();
		void Create(std::string shaderSrc);
		void Dispatch(unsigned int workGroupsX, unsigned int workGroupsY, unsigned int workGroupsZ);
		void Block();
		void SetFloat(std::string name, float value);
		void SetFloat2(std::string name, float value[2]);
		void SetFloat3(std::string name, float value[3]);
		void SetFloat4(std::string name, float value[4]);
		void SetInt(std::string name, int value);
		void SetInt2(std::string name, int value[2]);
		void SetInt3(std::string name, int value[3]);
		void SetInt4(std::string name, int value[4]);
		void SetUint(std::string name, unsigned int value);
		void SetUint2(std::string name, unsigned int value[2]);
		void SetUint3(std::string name, unsigned int value[3]);
		void SetUint4(std::string name, unsigned int value[4]);
		void SetMat2(std::string name, float value[4]);
		void SetMat3(std::string name, float value[9]);
		void SetMat4(std::string name, float value[16]);
		template<class T> void SetStorageBuffer(std::string name, StorageBuffer<T>& storageBuffer);
		void SetImage(std::string name, ShaderImage& shaderImage);
		void SetTexture(std::string name, Texture& texture, int textureUnit);
};


template<class T>
void ComputeShader::SetStorageBuffer(std::string name, StorageBuffer<T>& storageBuffer)
{
	GLuint blockIndex = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, name.c_str());
	glShaderStorageBlockBinding(computeProgram, blockIndex, storageBuffer.GetBindingPoint());
}


}//GLComputeHelper namespace 


#endif