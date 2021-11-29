#include "renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(std::shared_ptr<Camera> camera, std::shared_ptr<Sky> sky) : camera{ camera }, sky{ sky }
{
	
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

			glm::mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * object.getModelMatrix();
			shader->setUniformMat4("MVP", MVP);
			shader->setUniformMat4("modelMatrix", object.getModelMatrix());
			glm::mat4 VP = camera->getProjectionMatrix() * camera->getViewMatrix();
			shader->setUniformMat4("vpMatrix", VP);

			shader->setUniform3f("lightDir", glm::vec3(-3.0f, -4.0f, -4.0f));
			shader->setUniform4f("lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 5.0f));

			shader->setUniform3f("viewPos", camera->getPosition());

			shader->setUniform1f("time", time);

			glActiveTexture(GL_TEXTURE0 + object.getMaterial()->getTextureCount());
			shader->setUniform1i("irradianceMap", object.getMaterial()->getTextureCount());
			glBindTexture(GL_TEXTURE_CUBE_MAP, sky->getIrradiance());

			if (enableTessellation)
			{
				shader->setUniform1i("tessLevel", 1);
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