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
	
	void createQuad();
	GLuint quadVAO;
};