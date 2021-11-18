#include "renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(std::shared_ptr<Camera> camera) : camera{ camera }
{
	projectionMatrix = glm::perspective(glm::radians(70.0f), 1280.0f / 720.0f, 0.3f, 1000.0f);
}

void Renderer::begin(GLuint frameBuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::end()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::drawObjects(std::vector<RenderObject>& objects, float time)
{
	for (RenderObject& object : objects)
	{
		if (object.isEnabled())
		{
			std::shared_ptr<Shader> shader = object.getMaterial()->getShader();
			object.getMaterial()->bind();

			glm::mat4 MVP = projectionMatrix * camera->getViewMatrix() * object.getModelMatrix();
			shader->setUniformMat4("MVP", MVP);
			shader->setUniformMat4("modelMatrix", object.getModelMatrix());
			glm::mat4 VP = projectionMatrix * camera->getViewMatrix();
			shader->setUniformMat4("vpMatrix", VP);

			shader->setUniform3f("lightDir", glm::vec3(-3.0f, -4.0f, -4.0f));
			shader->setUniform4f("lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 5.0f));

			shader->setUniform3f("viewPos", camera->getPosition());

			shader->setUniform1f("time", time);

			if (enableTessellation)
			{
				shader->setUniform1i("tessLevel", 16);
				glPatchParameteri(GL_PATCH_VERTICES, patchSize);
				object.draw(GL_PATCHES);
			}
			else
			{
				object.draw(GL_TRIANGLES);
			}

			object.getMaterial()->unbind();
		}		
	}
}

void Renderer::drawObjects(std::vector<ParticleSystem>& objects, float time)
{
	for (ParticleSystem& system : objects)
	{
		std::vector<RenderObject> renderObjects = system.getRenderObjects();
		drawObjects(renderObjects, time);
	}
}