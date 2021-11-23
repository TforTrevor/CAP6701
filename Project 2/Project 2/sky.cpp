#include "sky.hpp"

Sky::Sky()
{
	std::shared_ptr<Model> cubeModel = std::make_shared<Model>("models/cube.obj", false);
	std::shared_ptr<Shader> equiShader = std::make_shared<Shader>("shaders/equi_to_cube.vert", "shaders/equi_to_cube.frag");
	std::shared_ptr<Texture> equiTexture = std::make_shared<Texture>("textures/spruit_sunrise_2k.exr");
	std::shared_ptr<Material> equiMaterial = std::make_shared<Material>(equiTexture, equiShader);

	skyboxObject = std::make_unique<RenderObject>(cubeModel, equiMaterial);

	captureCubemap();
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

void Sky::captureCubemap()
{
	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
	std::shared_ptr<Shader> skyboxShader = skyboxObject->getMaterial()->getShader();
	skyboxObject->getMaterial()->bind();
	//skyboxShader->setUniform1i("equirectangularMap", 0);
	skyboxShader->setUniformMat4("projectionMatrix", captureProjection);

	glViewport(0, 0, 512, 512);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (int i = 0; i < 6; i++)
	{
		skyboxShader->setUniformMat4("viewMatrix", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skyboxObject->draw(GL_TRIANGLES);
	}
	skyboxObject->getMaterial()->unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::shared_ptr<Model> cube = std::make_shared<Model>("models/cube.obj", false);
	std::shared_ptr<Shader> cubemapShader = std::make_shared<Shader>("shaders/skybox.vert", "shaders/skybox.frag");
	std::shared_ptr<Material> cubemapMaterial = std::make_shared<Material>(envCubemap, cubemapShader);
	skyboxObject = std::make_unique<RenderObject>(cube, cubemapMaterial);
}