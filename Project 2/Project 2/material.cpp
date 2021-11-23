#include "material.hpp"

Material::Material(std::shared_ptr<Texture> albedo, std::shared_ptr<Shader> shader) : albedoTexture{ albedo }, shader { shader }
{

}

Material::Material(GLuint textureID, std::shared_ptr<Shader> shader) : shader{ shader }
{
	albedoTexture = std::make_shared<Texture>(textureID);
}

void Material::bind()
{
	shader->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedoTexture->getID());
}

void Material::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	shader->unbind();
}