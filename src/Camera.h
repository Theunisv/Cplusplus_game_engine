#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <glfw3.h>
#include "glm/gtc/matrix_transform.hpp"
class Camera
{
private:
	GLfloat cameraSpeed;
	glm::mat4 camProjection;
	glm::mat4 camView;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	GLfloat deltaTime;
	GLfloat lastFrame;

	void translate(float value, std::string axis);
	void rotate(float value, std::string axis);
public:
	Camera(glm::mat4 projection);
	~Camera();

	void OnUpdate(bool keys[1024]);

	glm::mat4 getView();
	glm::mat4 getProjection();

	void setProjection(glm::mat4 projection);
};

