#pragma once

#include "shader.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "texture.hpp"

#include "GL/glew.h"
#include <vector>

class Trefoil
{
public:
	Trefoil();

	void draw(std::shared_ptr<Camera>& camera, bool uvMode);

private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Shader shader{ "shaders/trefoil.vert", "shaders/trefoil.frag" };
	Texture texture{ "textures/rainbow.png" };

	GLuint vertexArrayId;
	GLuint vertexBufferId;
	GLuint indexBufferId;

	void createVAO();
};