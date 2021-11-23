#pragma once

#include "render_object.hpp"
#include "camera.hpp"

class Sky
{
public:
	Sky();

	void draw(std::shared_ptr<Camera> camera);

private:
	std::unique_ptr<RenderObject> skyboxObject;

	void captureCubemap();
};