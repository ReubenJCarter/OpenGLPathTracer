#include "Context.h"
#include "Util.h"

#include <iostream>
#include <string> 


#define LOGD std::cout << "DEBUG:GLComputeHelper::Context:" 
#define LOGE std::cout << "ERROR:GLComputeHelper::Context:" 


/**
 *
 *GLComputeHelper namespace 
 *
 */
 

namespace GLComputeHelper
{

	
/**
 *
 *Context Class
 *
 */
 
 
bool Context::Init()
{
	int glfwInitOK = glfwInit();
	
	if(!glfwInitOK)
	{
		LOGE << "Could not init GLFW" << std::endl;
		return false;
	}
	return true;
}


bool Context::Destroy()
{
	glfwTerminate();	
	return true;
}

 
Context::Context(int w, int h, int antialiasing, bool fullscreen, std::string title)
{
	width = w;
	height = h;
	aspect = (float)w / (float)h;
	
	if(antialiasing > 0)
		glfwWindowHint(GLFW_SAMPLES, antialiasing);
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
	if(width <= 0 || height <= 0)
	{
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
		glfwWindow = glfwCreateWindow(1, 1, title.c_str(), NULL, NULL);
		glfwHideWindow(glfwWindow);
	}
	else
	{
		if(fullscreen)
			glfwWindow = glfwCreateWindow(width, height, title.c_str(), glfwGetPrimaryMonitor(), NULL);
		else
			glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	}
	
	if(!glfwWindow)
	{
		LOGE << "glfw3 window create failed" << std::endl;
		return;
	}
		
	//make the new window the current context be default
	MakeCurrent();
	
	SetSwapInterval(0);
	
	//init glew
	GLenum glewInitStatus = glewInit();
	if (GLEW_OK != glewInitStatus)
	{
		LOGE << "glew could not be inited:" << glewGetErrorString(glewInitStatus) << std::endl;
		return;
	}
	
	LOGD << "GLEW OK, version:" << glewGetString(GLEW_VERSION) << std::endl;
	if(GL_VERSION_4_5)
	{
		LOGD << "GL Version 4.5 support: OK" << std::endl;
	}
	else
	{
		LOGE << "no GL Version 4.5 supported: FAIL" << std::endl;
		return;
	}
	
	
	GLFWwindow* checkWindow = glfwGetCurrentContext();
	if(checkWindow == NULL)
	{
		LOGE << "NOT ABLE TO CREATE CONTEXT" << std::endl;
		return;
	}
	LOGD << "Start GL ERROR:" << GetGLError() << std::endl;
	LOGD << "Width " << width << " Height" << height << std::endl;	
	glGetIntegerv(GL_MAJOR_VERSION, &glVerMaj);
	glGetIntegerv(GL_MINOR_VERSION, &glVerMin);
	const char* shaderLangVerg = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	const char* vendorStr = (const char*)glGetString(GL_VENDOR);
	LOGD << "GLVersion " << glVerMaj << "." << glVerMin << std::endl;
	LOGD << "GLVersion2 " << glGetString(GL_VERSION) << std::endl;
	LOGD << "Shading Language version " << shaderLangVerg << "." << glVerMin << std::endl;
	LOGD << "Vendor " << vendorStr << std::endl;
	
	//SSBO parameters
	int val;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &val);
	LOGD << "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS:" << val << std::endl;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &val);
	LOGD << "GL_MAX_SHADER_STORAGE_BLOCK_SIZE:" << val << std::endl;
	glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &val);
	LOGD << "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS:" << val << std::endl;
	glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &val);
	LOGD << "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS:" << val << std::endl;
	glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &val);
	LOGD << "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS:" << val << std::endl;
	glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &val);
	LOGD << "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS:" << val << std::endl;
	glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &val);
	LOGD << "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS:" << val << std::endl;
	glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &val);
	LOGD << "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS:" << val << std::endl;
	glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &val);
	LOGD << "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS:" << val << std::endl;
	glGetIntegerv(GL_MAX_COMPUTE_IMAGE_UNIFORMS, &val);
	LOGD << "GL_MAX_COMPUTE_IMAGE_UNIFORMS:" << val << std::endl;	
	
	//compute shader parameters
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &val);
	LOGD << "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS:" << val << std::endl;
	int x, y, z;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &x);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &y);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &z);
	LOGD << "GL_MAX_COMPUTE_WORK_GROUP_COUNT:" << x << "," << y << "," << z << std::endl;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &x);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &y);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &z);
	LOGD << "GL_MAX_COMPUTE_WORK_GROUP_SIZE:" << x << "," << y << "," << z << std::endl;
}

Context::~Context()
{
	glfwDestroyWindow(glfwWindow);
}

void Context::MakeCurrent()
{
	glfwMakeContextCurrent(glfwWindow);
}


void Context::SetSwapInterval(int si)
{
	glfwSwapInterval(si);
}


void Context::SwapBuffers()
{
	glfwSwapBuffers(glfwWindow);
}


void Context::PollEvents()
{
	glfwPollEvents();
}


GLFWwindow* Context::GetGLFWWindow()
{
	return glfwWindow;
}


}//GLComputeHelper namespace 