#pragma once

#include "material.hpp"
#include "model.hpp"

class RenderObject
{
public:
	RenderObject(std::shared_ptr<Model> model, std::shared_ptr<Material> material);

	void draw(GLenum mode);
	void setPosition(glm::vec3 newPos);
	glm::mat4 getModelMatrix() { return modelMatrix; }
	void setEnabled(bool value) { enabled = value; }
	bool isEnabled() { return enabled; }
	std::shared_ptr<Material> getMaterial() { return material; }
private:
	std::shared_ptr<Model> model;
	std::shared_ptr<Material> material;

	int objectId;
	GLuint vertexArrayId;
	GLuint vertexBufferId;
	GLuint indexBufferId;

	glm::vec3 position;
	glm::mat4 modelMatrix = glm::mat4(1.0);

	bool enabled = true;

	void createVAO();
};