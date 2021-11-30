#include "sky.hpp"

Sky::Sky(std::string hdriPath)
{
	std::shared_ptr<Shader> equiShader = std::make_shared<Shader>("shaders/cubemap.vert", "shaders/equi_to_cube.frag");
	std::shared_ptr<Texture> equiTexture = std::make_shared<Texture>(hdriPath);
	std::shared_ptr<Material> equiMaterial = std::make_shared<Material>(equiTexture, equiShader);
	GLuint skyboxCubemap = captureCubemap(equiMaterial, 512, 512);

	std::shared_ptr<Shader> irradianceShader = std::make_shared<Shader>("shaders/cubemap.vert", "shaders/irradiance_convolution.frag");
	std::shared_ptr<Texture> irradianceTexture = std::make_shared<Texture>(skyboxCubemap, true);
	std::shared_ptr<Material> irradianceMaterial = std::make_shared<Material>(irradianceTexture, irradianceShader);
	irradianceMap = captureCubemap(irradianceMaterial, 32, 32);

	std::shared_ptr<Model> cubeModel = std::make_shared<Model>("models/cube.obj", false);
	std::shared_ptr<Shader> skyboxShader = std::make_shared<Shader>("shaders/skybox.vert", "shaders/skybox.frag");
	std::shared_ptr<Texture> skyboxTexture = std::make_shared<Texture>(skyboxCubemap, true);
	std::shared_ptr<Material> skyboxMaterial = std::make_shared<Material>(skyboxTexture, skyboxShader);
	skyboxObject = std::make_unique<RenderObject>(cubeModel, skyboxMaterial);
}

void Sky::draw(std::shared_ptr<Camera> camera)
{
	glDepthFunc(GL_LEQUAL);
	skyboxObject->getMaterial()->bind();
	skyboxObject->getMaterial()->getShader()->setUniformMat4("projectionMatrix", camera->getProjectionMatrix());
	skyboxObject->getMaterial()->getShader()->setUniformMat4("viewMatrix", camera->getViewMatrix());
	skyboxObject->draw(GL_TRIANGLES);
	skyboxObject->getMaterial()->unbind();
}

GLuint Sky::captureCubemap(std::shared_ptr<Material> material, const unsigned int width, const unsigned int height)
{
	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// convert HDR equirectangular environment map to cubemap equivalent
	std::shared_ptr<Model> cubeModel = std::make_shared<Model>("models/cube.obj", false);
	RenderObject renderObject{ cubeModel, material };

	renderObject.getMaterial()->bind();
	renderObject.getMaterial()->getShader()->setUniformMat4("projectionMatrix", captureProjection);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (int i = 0; i < 6; i++)
	{
		renderObject.getMaterial()->getShader()->setUniformMat4("viewMatrix", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderObject.draw(GL_TRIANGLES);
	}
	renderObject.getMaterial()->unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return envCubemap;
}