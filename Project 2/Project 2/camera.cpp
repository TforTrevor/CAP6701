#include "camera.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const unsigned int width, const unsigned int height) : CAMERA_WIDTH{width}, CAMERA_HEIGHT{height}
{
	position = glm::vec3(0, 0, -1);
	forward = glm::vec3(0, 0, 1);
	up = glm::vec3(0, 1, 0);
	right = glm::vec3(1, 0, 0);

	projectionMatrix = glm::perspective(glm::radians(fieldOfView), (float)width / (float)height, 0.3f, 1000.0f);
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + forward, up);
}

glm::mat4 Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::processInput(GLFWwindow* window, float deltaTime)
{
	float speed = 5.0f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 5.0f * 10.0f * deltaTime;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 5.0f * deltaTime;
	}	

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += forward * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position -= forward * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += right * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position -= right * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		position += glm::vec3(0, 1, 0) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		position -= glm::vec3(0, 1, 0) * speed;
	}
}

void Camera::processMouse(double x, double y)
{
	double xDelta = x - previousMouseX;
	double yDelta = y - previousMouseY;
	previousMouseX = x;
	previousMouseY = y;

	const float sensitivity = 0.1f;

	pitch -= yDelta * sensitivity;
	yaw += xDelta * sensitivity;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward.y = sin(glm::radians(pitch));
	forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
	up = glm::normalize(glm::cross(right, forward));
}