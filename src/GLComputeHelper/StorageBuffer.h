#ifndef STORAGE_BUFFER_H
#define STORAGE_BUFFER_H

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
 *StorageBuffer class 
 *
 */
 
 
template<class T>
class StorageBuffer
{
	private:
		unsigned int elementCount;
		GLuint storageBuffer;
		GLuint bindingPoint;
		
	public:
		enum Access{READ, WRITE, READ_WRITE};
	
		StorageBuffer();
		StorageBuffer(unsigned int count, unsigned int bindingP, T* data=NULL);
		~StorageBuffer();
		void Allocate(unsigned int count, unsigned int bindingP, T* data=NULL);
		void Deallocate();
		void Copy(T* data);
		void Get(T* data);
		unsigned int GetBindingPoint();		
		unsigned int Size();
};


template <class T>
StorageBuffer<T>::StorageBuffer()
{
	elementCount = 0;
}


template <class T>
StorageBuffer<T>::StorageBuffer(unsigned int count, unsigned int bindingP, T* data)
{
	Allocate(count, bindingP, data);
}


template <class T>
StorageBuffer<T>::~StorageBuffer()
{
	glDeleteBuffers(1, &storageBuffer);
}


template <class T>
void StorageBuffer<T>::Allocate(unsigned int count, unsigned int bindingP, T* data)
{
	elementCount = count;
	glGenBuffers(1, &storageBuffer);
	bindingPoint = bindingP;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, storageBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * count, data, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, storageBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


template <class T>
void StorageBuffer<T>::Deallocate()
{
	glDeleteBuffers(1, &storageBuffer);
}


template <class T>
void StorageBuffer<T>::Copy(T* data)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, storageBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * elementCount, data, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


template <class T>
void StorageBuffer<T>::Get(T* data)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, storageBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(T) * elementCount, data);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


template <class T>
unsigned int StorageBuffer<T>::GetBindingPoint()
{
	return bindingPoint;
}


template <class T>
unsigned int StorageBuffer<T>::Size()
{
	return elementCount;
}


}//GLComputeHelper namespace 


#endif