#include "Util.h"

/**
 *
 *GLComputeHelper namespace 
 *
 */
 

namespace GLComputeHelper
{
	

/**
 *
 *GetGLError function
 *
 */


std::string GetGLError()
{
	std::string err = "";
	GLenum glError = glGetError();
	if(glError == GL_INVALID_ENUM) err = "GL_INVALID_ENUM";
	else if(glError == GL_INVALID_VALUE) err = "GL_INVALID_VALUE";
	else if(glError == GL_INVALID_OPERATION) err = "GL_INVALID_OPERATION";
	else if(glError == GL_STACK_OVERFLOW) err = "GL_STACK_OVERFLOW";
	else if(glError == GL_STACK_UNDERFLOW) err = "GL_STACK_UNDERFLOW";
	else if(glError == GL_OUT_OF_MEMORY) err = "GL_OUT_OF_MEMORY";
	else if(glError == GL_INVALID_FRAMEBUFFER_OPERATION) err = "GL_INVALID_FRAMEBUFFER_OPERATION";
	else if(glError == GL_TABLE_TOO_LARGE) err = "GL_TABLE_TOO_LARGE";
	else err = "NO ERROR";
	return err;
}


}//GLComputeHelper namespace 

