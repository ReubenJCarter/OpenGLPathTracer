#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


/**
 *
 *RenderEngine Namespace
 *
 */


namespace RenderEngine
{


/**
 *
 *Camera Class
 *
 */


class Camera
{
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		float fov;
		
		Camera(glm::vec3 p=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 r=glm::vec3(0.0f, 0.0f, 0.0f), float f=1.57);
		void GetRotation(float* rotM);
		float GetLength();
};


}//RenderEngine Namespace


#endif