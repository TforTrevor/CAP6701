#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera(const unsigned int width, const unsigned int height);

	void processInput(GLFWwindow* window, float deltaTime);
	void processMouse(double x, double y);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::vec3 getPosition() { return position; }
private:
	glm::vec3 position;
	float pitch = 0;
	float yaw = 0;
	float roll = 0;

	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	double previousMouseX = 0;
	double previousMouseY = 0;

	float fieldOfView = 70.0f;
	glm::mat4 projectionMatrix;
};