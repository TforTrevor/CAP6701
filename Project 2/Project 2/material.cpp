#include "material.hpp"

Material::Material(std::shared_ptr<Texture> albedo, std::shared_ptr<Shader> shader) : albedoTexture{ albedo }, shader { shader }
{

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