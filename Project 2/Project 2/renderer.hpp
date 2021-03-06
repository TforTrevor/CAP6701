#pragma once

#include "render_object.hpp"
#include "camera.hpp"
#include "particle_system.hpp"
#include "sky.hpp"

#include <string>
#include <memory>

class Renderer
{
public:
	Renderer(std::shared_ptr<Camera> camera, std::shared_ptr<Sky> sky);

	void begin(GLuint frameBuffer);
	void drawObjects(std::vector<RenderObject>& objects, float time);
	void drawObjects(std::vector<ParticleSystem>& objects, float time);
	void end();

	void setPatchSize(int size) { patchSize = size; }
	void toggleTessellation(bool value) { enableTessellation = value; }

private:
	std::shared_ptr<Camera> camera;
	std::shared_ptr<Sky> sky;

	bool enableTessellation = false;
	int patchSize = 3;
};