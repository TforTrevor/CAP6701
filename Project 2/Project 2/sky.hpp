#pragma once

#include "render_object.hpp"
#include "camera.hpp"

class Sky
{
public:
	Sky(std::string hdriPath);

	void draw(std::shared_ptr<Camera> camera);
	GLuint getIrradiance() { return irradianceMap; }
	GLuint getPrefilter() { return prefilterMap; }
	GLuint getBRDF() { return brdfMap; }

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
	void captureIrradiance(const glm::mat4& projection, const glm::mat4 view[], int width, int height);
	void capturePrefilter(const glm::mat4& projection, const glm::mat4 view[], int width, int height);
	void captureBRDF(int width, int height);
	GLuint captureLUT(Shader& lutShader, int width, int height, int stepCount);
	GLuint captureLUT(Shader& lutShader, int width, int height, int depth, int stepCount);
	
	void createQuad();
	GLuint quadVAO;

	//PBR sky
	GLuint transmittanceLUT;
	GLuint skyViewLUT;
	GLuint aerialPerspectiveLUT;
	GLuint multiScatteringLUT;
	GLuint pbrSkyMap;

	void captureTransmittance(int width, int height, int stepCount);
	void captureMultiScatter(int width, int height, int stepCount);

	glm::vec3 sunDirection = glm::vec3(-3.0f, -4.0f, -4.0f);
};