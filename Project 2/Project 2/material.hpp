#pragma once

#include "shader.hpp"
#include "texture.hpp"

#include <memory>

class Material
{
public:
	Material(std::shared_ptr<Texture> albedo, std::shared_ptr<Shader> shader);

	void bind();
	void unbind();
	std::shared_ptr<Shader> getShader() { return shader; }

	float metallic = 0.0f;
	float roughness = 0.5f;
private:
	std::shared_ptr<Shader> shader;

	std::shared_ptr<Texture> albedoTexture;

	//GLuint normalTexture;
	//GLuint maskTexture; //r: metal, g: ao, b: null, a: roughness
	GLuint metallicID;
	GLuint roughnessID;
};