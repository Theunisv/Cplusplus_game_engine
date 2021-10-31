#include "Camera.h"

void Camera::translate(float value, std::string axis)
{
}

void Camera::rotate(float value, std::string axis)
{
}

Camera::Camera(glm::mat4 projection)
	: cameraSpeed(0.05f), camProjection(projection), camView(0), cameraPos(glm::vec3(0.0f, 0.0f, 3.0f)), cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)), deltaTime(0.0f), lastFrame(0.0f)
{
	camView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

Camera::~Camera()
{
}

void Camera::OnUpdate(bool keys[1024])
{	
	GLfloat currentFrame = glfwGetTime(); 
	deltaTime = currentFrame - lastFrame; 
	lastFrame = currentFrame;

	cameraSpeed = 5.0f * deltaTime;

	if (keys[GLFW_KEY_W])cameraPos += cameraSpeed * cameraFront; 
	if (keys[GLFW_KEY_S])cameraPos -= cameraSpeed * cameraFront; 
	if (keys[GLFW_KEY_A])cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; 
	if (keys[GLFW_KEY_D])cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	camView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::getView()
{
	return glm::mat4();
}

glm::mat4 Camera::getProjection()
{
	return glm::mat4();
}

void Camera::setProjection(glm::mat4 projection)
{
}
