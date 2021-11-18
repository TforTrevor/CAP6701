#pragma once

#include "shader.hpp"
#include "hdr_fbo.hpp"

#include <memory>
#include <queue>

class PostProcessing
{
public:
	PostProcessing(const unsigned int width, const unsigned int height);

	void begin(const HDRFBO& hdrFBO);
	void render(std::shared_ptr<Shader> effect);
	void end();

private:
	GLuint quadVAO;
	GLuint quadVBO;

	Shader copyShader{ "shaders/post_processing.vert", "shaders/copy.frag" };

	std::queue<HDRFBO> hdrFBOs;

	void createQuad();
};