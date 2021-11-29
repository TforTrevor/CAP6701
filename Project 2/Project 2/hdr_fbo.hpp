#pragma once

#include "GL/glew.h"

#include <iostream>

struct HDRFBO
{
	GLuint frameBufferObject;
	GLuint colorBuffer;
	GLuint renderBuffer;

	const unsigned int WIDTH;
	const unsigned int HEIGHT;

	HDRFBO(const unsigned int width, const unsigned int height) : WIDTH{width}, HEIGHT{height}
	{
		if (width > 0 && height > 0)
		{
			glGenFramebuffers(1, &frameBufferObject);

			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glGenRenderbuffers(1, &renderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}		
	}
};