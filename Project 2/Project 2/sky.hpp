#pragma once

#include "render_object.hpp"
#include "camera.hpp"

class Sky
{
public:
	struct AtmosphereProperties
	{
		float groundRadius = 6.360f;
		float atmosphereRadius = 6.460f;
		glm::vec3 rayleighScatteringBase = glm::vec3(5.802f, 13.558f, 33.100f);
		float rayleighAbsorptionBase = 0.0f;
		float mieScatteringBase = 3.996f;
		float mieAbsorptionBase = 4.400f;
		glm::vec3 ozoneAbsorptionBase = glm::vec3(0.650f, 1.881f, 0.085f);
		glm::vec3 groundAlbedo = glm::vec3(0.3f, 0.3f, 0.3f);

		glm::vec2 skyViewSize = glm::vec2(200, 100);
	};

public:
	Sky(std::string hdriPath);

	void draw(std::shared_ptr<Camera> camera);
	void drawPBR(AtmosphereProperties& properties, std::shared_ptr<Camera> camera, float time);
	GLuint getIrradiance() { return irradianceMap; }
	GLuint getPrefilter() { return prefilterMap; }
	GLuint getBRDF() { return brdfMap; }
	glm::vec3 getSunDirection() { return sunDirection; }

private:
	std::string hdriPath;
	std::unique_ptr<RenderObject> skyboxObject;
	GLuint captureFBO;
	GLuint hdriMap;
	GLuint irradianceMap;
	GLuint prefilterMap;
	GLuint brdfMap;

	GLuint createCubemap(int width, int height, bool mipMaps);
	void captureSky(const glm::mat4& projection, const glm::mat4 view[], int width, int height);
	void captureIrradiance(GLuint environmentMap, int width, int height);
	void capturePrefilter(GLuint environmentMap, int width, int height);
	void captureBRDF(int width, int height);
	
	void createQuad();
	GLuint quadVAO;

	//PBR sky
	GLuint transmittanceLUT;
	GLuint aerialPerspectiveLUT;
	GLuint multiScatteringLUT;
	GLuint skyViewLUT;
	GLuint pbrSkyMap;

	void captureTransmittance(int width, int height, int stepCount);
	void captureMultiScatter(int width, int height, int stepCount);
	void captureSkyView(int width, int height, int stepCount, std::shared_ptr<Camera> camera, float time);
	void captureSkyPBR(int width, int height, std::shared_ptr<Camera> camera, float time);
	void setAtmosphereProperties(Shader& shader);

	Shader transmittanceShader{ "shaders/post_processing.vert", "shaders/sky/transmittance.frag" };
	Shader multiScatteringShader{ "shaders/post_processing.vert", "shaders/sky/multi_scattering.frag" };
	Shader skyViewShader{ "shaders/post_processing.vert", "shaders/sky/sky_view.frag" };
	Shader finalShader{ "shaders/sky/sky_plane.vert", "shaders/sky/final.frag" };

	glm::vec3 sunDirection = glm::vec3(-3.0f, -4.0f, -4.0f);

	glm::vec2 transmittanceLUTSize = glm::vec2(-1, -1);
	glm::vec2 multiScatterLUTSize = glm::vec2(-1, -1);
	glm::vec2 skyViewLUTSize = glm::vec2(-1, -1);
	glm::vec2 pbrSkySize = glm::vec2(-1, -1);
	glm::vec2 irradianceSize = glm::vec2(-1, -1);
	glm::vec2 prefilterSize = glm::vec2(-1, -1);
	glm::vec2 pbrSkyMapSize = glm::vec2(-1, -1);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[6] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	Shader irradianceShader{ "shaders/cubemap.vert", "shaders/irradiance_convolution.frag" };
	Shader prefilterShader{ "shaders/cubemap.vert", "shaders/prefilter.frag" };

	std::shared_ptr<Model> cubeModel = std::make_shared<Model>("models/cube.obj", false);
	RenderObject cubeObject{ cubeModel };

	float currentTime = -1.0f;

	AtmosphereProperties atmosphereProperties{};
};