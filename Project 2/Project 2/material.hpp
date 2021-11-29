#pragma once

#include "shader.hpp"
#include "texture.hpp"

#include <memory>

class Material
{
public:
	Material(std::shared_ptr<Texture> albedo, std::shared_ptr<Shader> shader);
	//Material(GLuint textureID, std::shared_ptr<Shader> shader);

	void bind();
	void unbind();
	std::shared_ptr<Shader> getShader() { return shader; }
	unsigned int getTextureCount() { return textureCount; }

	float metallic = 0.0f;
	float roughness = 0.35f;
private:
	std::shared_ptr<Shader> shader;

	std::shared_ptr<Texture> albedoTexture;

	unsigned int textureCount = 1;
};