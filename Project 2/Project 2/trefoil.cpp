#include "trefoil.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

Trefoil::Trefoil()
{
	//indices = {
	//	0, 1, 2, 3,
	//	1, 2, 3, 4,
	//	2, 3, 4, 5,
	//	3, 4, 5, 0,
	//	4, 5, 0, 1,
	//	5, 0, 1, 2
	//};

	const int resolution = 128;
	const float size = 1.0f;

	for (int i = 0; i < resolution; i++)
	{
		float t = (float)i / float(resolution) * 2 * M_PI;

		float x = sin(t) + 2 * sin(2 * t);
		float y = cos(t) - 2 * cos(2 * t);
		float z = -sin(3 * t);

		Vertex vertex = {
			{ x * size, -y * size, z * size },
			{ 1, 1, 1 },
			{ 1, 1, 1 },
			{ (float)i / float(resolution), 0.5f }
		};

		vertices.push_back(vertex);
		indices.push_back(i);
	}
	indices.push_back(0);

	createVAO();
}

void Trefoil::createVAO()
{
	//Create VAO
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	//Create buffer for vertex data
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	//Create buffer for indices
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

	//Assign vertex attributes
	size_t normalOffset = sizeof(glm::vec3);
	size_t colorOffset = normalOffset + sizeof(glm::vec3);
	size_t uvOffset = colorOffset + sizeof(glm::vec3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)normalOffset);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)colorOffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)uvOffset);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

void Trefoil::draw(std::shared_ptr<Camera>& camera, bool uvMode)
{
	shader.bind();
	glm::mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * glm::mat4(1);
	shader.setUniformMat4("MVP", MVP);
	shader.setUniformMat4("modelMatrix", glm::mat4(1));
	shader.setUniform1i("uvMode", uvMode);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.getID());

	glBindVertexArray(vertexArrayId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());
	glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, (void*)0);

	shader.unbind();
}