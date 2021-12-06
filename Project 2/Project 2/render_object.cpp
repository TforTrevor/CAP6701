#include "render_object.hpp"

RenderObject::RenderObject(std::shared_ptr<Model> model) : model{ model }
{
	createVAO();
}

RenderObject::RenderObject(std::shared_ptr<Model> model, std::shared_ptr<Material> material) : model { model }, material{material}
{
	createVAO();
}

void RenderObject::createVAO()
{
	//Create VAO
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	//Create buffer for vertex data
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model->vertices[0]) * model->vertices.size(), model->vertices.data(), GL_STATIC_DRAW);

	//Create buffer for indices
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model->indices[0]) * model->indices.size(), model->indices.data(), GL_STATIC_DRAW);

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

void RenderObject::draw(GLenum mode)
{
	if (enabled)
	{
		if (material != nullptr)
		{
			material->bind();
		}
		
		glBindVertexArray(vertexArrayId);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * model->vertices.size(), model->vertices.data());
		glDrawElements(mode, model->indices.size(), GL_UNSIGNED_INT, (void*)0);
	}	
}

void RenderObject::setPosition(glm::vec3 newPos)
{
	modelMatrix = glm::translate(glm::mat4(1.0), newPos);
	position = newPos;
}