#include "sky.hpp"

Sky::Sky(std::string hdriPath) : hdriPath{ hdriPath }
{
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

	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	captureSky(captureProjection, captureViews, 512, 512);
	captureIrradiance(captureProjection, captureViews, 32, 32);
	capturePrefilter(captureProjection, captureViews, 128, 128);
	captureBRDF(512, 512);

	std::shared_ptr<Model> cubeModel = std::make_shared<Model>("models/cube.obj", false);
	std::shared_ptr<Shader> skyboxShader = std::make_shared<Shader>("shaders/skybox.vert", "shaders/skybox.frag");
	std::shared_ptr<Texture> skyboxTexture = std::make_shared<Texture>(hdriMap, true);
	std::shared_ptr<Material> skyboxMaterial = std::make_shared<Material>(skyboxTexture, skyboxShader);
	skyboxObject = std::make_unique<RenderObject>(cubeModel, skyboxMaterial);

	
	Shader skyViewShader{ "shaders/post_processing.vert", "shaders/sky/sky_view.frag" };
	Shader aerialPerspectiveShader{ "shaders/post_processing.vert", "shaders/sky/aerial_perspective.frag" };
	
	Shader finalShader{ "shaders/post_processing.vert", "shaders/sky/final.frag" };

	captureTransmittance(256, 64, 40);
	//skyViewLUT = captureLUT(skyViewShader, 200, 100, 30);
	//aerialPerspectiveLUT = captureLUT(aerialPerspectiveShader, 32, 32, 32, 30);
	captureMultiScatter(32, 32, 20);
	//pbrSkyMap = 
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

GLuint Sky::createCubemap(int width, int height, bool mipMaps)
{
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
	if (mipMaps)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return envCubemap;
}

void Sky::captureSky(const glm::mat4& projection, const glm::mat4 view[], int width, int height)
{
	hdriMap = createCubemap(width, height, true);

	std::shared_ptr<Shader> equiShader = std::make_shared<Shader>("shaders/cubemap.vert", "shaders/equi_to_cube.frag");
	std::shared_ptr<Texture> equiTexture = std::make_shared<Texture>(hdriPath);
	std::shared_ptr<Material> equiMaterial = std::make_shared<Material>(equiTexture, equiShader);
	std::shared_ptr<Model> cubeModel = std::make_shared<Model>("models/cube.obj", false);
	RenderObject renderObject{ cubeModel, equiMaterial };

	equiMaterial->bind();
	equiShader->setUniformMat4("projectionMatrix", projection);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (int i = 0; i < 6; i++)
	{
		equiShader->setUniformMat4("viewMatrix", view[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, hdriMap, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		renderObject.draw(GL_TRIANGLES);
	}

	equiMaterial->unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, hdriMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Sky::captureIrradiance(const glm::mat4& projection, const glm::mat4 view[], int width, int height)
{
	irradianceMap = createCubemap(width, height, false);

	std::shared_ptr<Shader> irradianceShader = std::make_shared<Shader>("shaders/cubemap.vert", "shaders/irradiance_convolution.frag");
	std::shared_ptr<Texture> irradianceTexture = std::make_shared<Texture>(hdriMap, true);
	std::shared_ptr<Material> irradianceMaterial = std::make_shared<Material>(irradianceTexture, irradianceShader);
	std::shared_ptr<Model> cubeModel = std::make_shared<Model>("models/cube.obj", false);
	RenderObject renderObject{ cubeModel, irradianceMaterial };

	irradianceMaterial->bind();
	irradianceShader->setUniformMat4("projectionMatrix", projection);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (int i = 0; i < 6; i++)
	{
		irradianceShader->setUniformMat4("viewMatrix", view[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		renderObject.draw(GL_TRIANGLES);
	}

	irradianceMaterial->unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Sky::capturePrefilter(const glm::mat4& projection, const glm::mat4 view[], int width, int height)
{
	prefilterMap = createCubemap(width, height, true);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	std::shared_ptr<Shader> prefilterShader = std::make_shared<Shader>("shaders/cubemap.vert", "shaders/irradiance_convolution.frag");
	std::shared_ptr<Texture> prefilterTexture = std::make_shared<Texture>(hdriMap, true);
	std::shared_ptr<Material> prefilterMaterial = std::make_shared<Material>(prefilterTexture, prefilterShader);
	std::shared_ptr<Model> cubeModel = std::make_shared<Model>("models/cube.obj", false);
	RenderObject renderObject{ cubeModel, prefilterMaterial };

	prefilterMaterial->bind();
	prefilterShader->setUniformMat4("projectionMatrix", projection);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	const unsigned int maxMipLevels = 5;
	for (int mip = 0; mip < maxMipLevels; mip++)
	{
		unsigned int mipWidth = width * std::pow(0.5f, mip);
		unsigned int mipHeight = height * std::pow(0.5f, mip);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->setUniform1f("roughness", roughness);

		for (int i = 0; i < 6; i++)
		{
			prefilterShader->setUniformMat4("viewMatrix", view[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT);

			renderObject.draw(GL_TRIANGLES);
		}
	}

	prefilterMaterial->unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Sky::captureBRDF(int width, int height)
{
	glGenTextures(1, &brdfMap);

	glBindTexture(GL_TEXTURE_2D, brdfMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	createQuad();

	Shader brdfShader{ "shaders/post_processing.vert", "shaders/brdf.frag" };

	glViewport(0, 0, width, height);
	brdfShader.bind();

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfMap, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	brdfShader.unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Sky::createQuad()
{
	float quadVertices[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//UV
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void Sky::captureTransmittance(int width, int height, int stepCount)
{
	glGenTextures(1, &transmittanceLUT);
	glBindTexture(GL_TEXTURE_2D, transmittanceLUT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	Shader transmittanceShader{ "shaders/post_processing.vert", "shaders/sky/transmittance.frag" };

	glViewport(0, 0, width, height);
	transmittanceShader.bind();
	transmittanceShader.setUniform3f("sunDirection", glm::normalize(sunDirection));
	transmittanceShader.setUniform1i("stepCount", stepCount);
	transmittanceShader.setUniform2f("lutRes", glm::vec2(width, height));
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, transmittanceLUT, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	transmittanceShader.unbind();
}

void Sky::captureMultiScatter(int width, int height, int stepCount)
{
	glGenTextures(1, &multiScatteringLUT);
	glBindTexture(GL_TEXTURE_2D, multiScatteringLUT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	Shader multiScatteringShader{ "shaders/post_processing.vert", "shaders/sky/multi_scattering.frag" };

	glViewport(0, 0, width, height);
	multiScatteringShader.bind();
	multiScatteringShader.setUniform3f("sunDirection", glm::normalize(sunDirection));
	multiScatteringShader.setUniform1i("stepCount", stepCount);
	multiScatteringShader.setUniform2f("lutRes", glm::vec2(width, height));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, transmittanceLUT);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, multiScatteringLUT, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	multiScatteringShader.unbind();
}