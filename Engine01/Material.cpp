#include "Material.h"

Material::Material(Shader* shader, Texture* texture)
{
	this->lit = false;
	this->shader = shader;
	//viewMatrix = shader->getUniformLocation("viewMatrix");
	projectionViewMatrix = shader->getUniformLocation("projectionViewMatrix");
	modelMatrix = shader->getUniformLocation("modelMatrix");
	textureUniform = shader->getUniformLocation("tex");
	this->texture = texture;

}

void Material::setLit(bool val)
{
	lit = val;
	if (lit)
	{
		ambientUniform = shader->getUniformLocation("ambient");
		diffuseUniform = shader->getUniformLocation("diffuse");
		lightIntensityUniform = shader->getUniformLocation("lightIntensity");
		lightPositionUniform = shader->getUniformLocation("lightPosition");
		normalMatrix = shader->getUniformLocation("normalMatrix");
	}
}

void Material::use(Camera* camera)
{
	if (enableBlending)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	shader->use();
	shader->setUniform(textureUniform, 0);
	//shader->setUniform(viewMatrix, camera->getViewMatrix());

	shader->setUniform(projectionViewMatrix, camera->getProjectionMatrix() * camera->getViewMatrix());

	if (lit)
	{
		shader->setUniform(ambientUniform, Ka);
		shader->setUniform(diffuseUniform, Kd);
		shader->setUniform(lightIntensityUniform, vec3(0.8f, 0.8f, 0.8f));
		shader->setUniform(lightPositionUniform, glm::normalize(lightPos));
	}


	glActiveTexture(GL_TEXTURE0);
	texture->use();
}

void Material::setTransform(Transform* transform)
{
	//this function assumes that use() has already been called to activate the shader
	shader->setUniform(modelMatrix, transform->getMatrix());
	if (lit)
	{
		shader->setUniform(normalMatrix, transform->getNormalMatrix());
	}
}