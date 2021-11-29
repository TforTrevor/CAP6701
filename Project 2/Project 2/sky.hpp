#pragma once

#include "render_object.hpp"
#include "camera.hpp"

class Sky
{
public:
	Sky(std::string hdriPath);

	void draw(std::shared_ptr<Camera> camera);
	GLuint getIrradiance() { return irradianceMap; }

private:
	std::unique_ptr<RenderObject> skyboxObject;
	GLuint irradianceMap;

	GLuint captureCubemap(std::shared_ptr<Material> material, const unsigned int width, const unsigned int height);
};