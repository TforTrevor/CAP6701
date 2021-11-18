#pragma once

#include <GL/glew.h>

#include <string>

class Texture
{
public:
	Texture(std::string path);

	GLuint getID() { return id; }
private:
	GLuint id;
};