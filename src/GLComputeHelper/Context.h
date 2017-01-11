#ifndef CONTEXT_H
#define CONTEXT_H

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
 *Context Class
 *
 */

 
class Context
{
	private:
		GLFWwindow* glfwWindow;
		int width;
		int height;
		float aspect;
		int glVerMaj, glVerMin;
		
	public:	
		static bool Init();
		static bool Destroy();
		
		Context(int w = 0, int h = 0, int antialiasing = 0, bool fullscreen = false, std::string title = "Title");
		~Context();
		void MakeCurrent();
		void SetSwapInterval(int si);
		void SwapBuffers();
		void PollEvents();
		GLFWwindow* GetGLFWWindow();
};


}//GLComputeHelper namespace 

#endif