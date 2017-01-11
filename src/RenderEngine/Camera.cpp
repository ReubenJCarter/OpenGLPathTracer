#include "Camera.h"
#include <math.h>


/**
 *
 *RenderEngine Namespace
 *
 */


namespace RenderEngine
{


#define PI 3.14159265

	
/**
 *
 *Camera Class
 *
 */
 
	
Camera::Camera(glm::vec3 p, glm::vec3 r, float f)
{
	position = glm::vec3(0.0f, 0.0f, 1.0f);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	fov = PI / 2.0f;
}

void Camera::GetRotation(float* rotM)
{
	glm::mat4 rx = glm::rotate(glm::mat4(1.0), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ry = glm::rotate(glm::mat4(1.0), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rz = glm::rotate(glm::mat4(1.0), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 r = rx * ry * rz;
	rotM[0] = r[0][0]; rotM[3] = r[1][0]; rotM[6] = r[2][0];
	rotM[1] = r[0][1]; rotM[4] = r[1][1]; rotM[7] = r[2][1];
	rotM[2] = r[0][2]; rotM[5] = r[1][2]; rotM[8] = r[2][2];
}

float Camera::GetLength()
{
	return 1.0 / tan(fov / 2.0);
}
	
	
}//RenderEngine Namespace