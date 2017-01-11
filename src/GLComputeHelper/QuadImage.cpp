#include "QuadImage.h"

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream> 

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
 
 
std::string vShaderSrc = ""
"#version 430\n"
"out vec2 texco;"
"layout(location = 0) in vec4 vp;"
"void main() {"
"	texco = vec2(vp.x * 0.5f + 0.5f, vp.y * 0.5f + 0.5f);"
" 	gl_Position = vp;"
"}";


std::string fShaderSrc = ""
"#version 430\n"
"in vec2 texco;"
"uniform sampler2D tex;"
"uniform float intensity;"
"out vec4 frag_colour;"
"void main() {"
"  frag_colour = vec4(intensity * texture(tex, texco).xyz, 1.0f);"
"}";

 
bool QuadImage::inited = false;
unsigned int QuadImage::shaderProgram = 0;

void QuadImage::Init()
{
	const char* vsrc[1];
	vsrc[0] = vShaderSrc.c_str();
	const char* fsrc[1];
	fsrc[0] = fShaderSrc.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vsrc, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fsrc, NULL);
	glCompileShader(fs);
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);
	
	//check shader link for errors
	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);
		
		//We don't need the program anymore.
		glDeleteProgram(shaderProgram);
		//Don't leak shaders either.
		glDeleteShader(shaderProgram);

		//log the info
		std::cerr << &infoLog[0] << std::endl;
		
		//return;
	}
}

QuadImage::QuadImage(int w, int h)
{
	
	if(!inited)
	{
		Init();
		inited = true;
	}
	
	float points[] = 
	{
		1.0f, 1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  0.0f, 1.0f, 
		-1.0f,  -1.0f,  0.0f, 1.0f,
		-1.0f,  -1.0f,  0.0f, 1.0f,
		-1.0f, 1.0f,  0.0f, 1.0f,
		1.0f, 1.0f,  0.0f, 1.0f
	};
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), points, GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	
	intensity = 1.0f;
}

QuadImage::~QuadImage()
{
	glDeleteBuffers(1, &vbo);
}

void QuadImage::Draw(unsigned int glTexture)
{
	glUseProgram(shaderProgram);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glTexture);
	glProgramUniform1i(shaderProgram, glGetUniformLocation(shaderProgram, "tex"), 0);
	glProgramUniform1f(shaderProgram, glGetUniformLocation(shaderProgram, "intensity"), intensity);
	
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void QuadImage::SetIntensity(float i)
{
	intensity = i;
}


}//GLComputeHelper namespace


