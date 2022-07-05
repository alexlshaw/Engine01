#include "Transform.h"

Transform::Transform()
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);	//default rotation around y axis of 0 degrees
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	buildTransformMatrix();
	dirty = false;
}

void Transform::buildTransformMatrix()
{
	//takes the individual transformation properties of the object and combines them to form the transformation matrix
	glm::mat4 translationMatrix = glm::translate(position);
	glm::mat4 rotationMatrix = glm::rotate(rotation.w, glm::vec3(rotation.x, rotation.y, rotation.z));
	glm::mat4 scaleMatrix = glm::scale(scale);
	transform = translationMatrix * rotationMatrix * scaleMatrix;
	normal = glm::inverseTranspose(glm::mat3(transform));
	dirty = false;
}

glm::vec3 Transform::getPosition()
{
	return position;
}

void Transform::setPosition(glm::vec3 pos)
{
	dirty = true;
	position = pos;
}

glm::vec3 Transform::getScale()
{
	return scale;
}

void Transform::setScale(glm::vec3 scl)
{
	dirty = true;
	scale = scl;
}

glm::vec4 Transform::getRotation()
{
	return rotation;
}

void Transform::setRotation(glm::vec4 rot)
{
	dirty = true;
	rotation = rot;
}

glm::mat4 Transform::getMatrix()
{
	if (dirty)
	{
		buildTransformMatrix();
	}
	return transform;
}

glm::mat3 Transform::getNormalMatrix()
{
	if (dirty)
	{
		buildTransformMatrix();
	}
	return normal;
}