#include "material.hpp"

Material::Material(std::shared_ptr<Texture> albedo, std::shared_ptr<Shader> shader) : albedoTexture{ albedo }, shader { shader }
{

}

//Material::Material(GLuint textureID, std::shared_ptr<Shader> shader) : shader{ shader }
//{
//	albedoTexture = std::make_shared<Texture>(textureID);
//}

void Material::bind()
{
	shader->bind();
	shader->setUniform1f("roughness", roughness);
	shader->setUniform1f("metallic", metallic);
	glActiveTexture(GL_TEXTURE0);
	if (albedoTexture->isCubemap())
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, albedoTexture->getID());
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, albedoTexture->getID());
	}
	
}

void Material::unbind()
{
	if (albedoTexture->isCubemap())
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	shader->unbind();
}