#pragma once

#include <GL/glew.h>

#include <string>

class Texture
{
public:
	Texture(std::string path);
	Texture(GLuint textureID);

	void loadSTB(std::string path);
	void loadEXR(std::string path);

	GLuint getID() { return id; }
private:
	GLuint id;
};