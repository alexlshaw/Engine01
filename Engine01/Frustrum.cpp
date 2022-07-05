#include "Frustrum.h"

Frustrum::Frustrum(){}

Frustrum::~Frustrum(){}

void Frustrum::setCamInternals(float angle, float ratio, float nearDistance, float farDistance)
{
	//store the basic properties
	this->ratio = ratio;
	this->angle = angle;
	this->nearDistance = nearDistance;
	this->farDistance = farDistance;

	//compute the width and height of the near and far clipping planes
	tang = (float)(tan(DEGREES_TO_RADIANS * angle * 0.5f));
	nearHeight = nearDistance * tang;
	nearWidth = nearHeight * ratio;
	farHeight = farDistance * tang;
	farWidth = farHeight * ratio;
}

//position of camera, point where camera is looking, up vector
//near plane (and presumably far plane) are probably correct, but sides and top are not
void Frustrum::setCamDef(glm::vec3 &position, glm::vec3 &look, glm::vec3 &up)
{
	glm::vec3 nearCenter, farCenter, X, Y, Z;
	//compute z axis of camera
	//this axis points in the opposite direction from the looking direction
	Z = position - look;	//can probably just use -camera.direction
	Z = glm::normalize(Z);

	//x axis of camera with given "up" vector and Z axis (camera.right)
	X = glm::cross(up, Z);
	X = glm::normalize(X);

	//the real up vector is the cross product of Z and X (do I need to use glm::cross?)
	Y = glm::normalize(glm::cross(Z, X));
	

	//compute the centers of the near and far planes
	nearCenter = position - Z * nearDistance;
	farCenter = position - Z * farDistance;

	//compute planes
	pl[NEARP].setNormalAndPoint(-Z, nearCenter);
	pl[FARP].setNormalAndPoint(Z, farCenter);
	glm::vec3 aux, normal;
	
	aux = (nearCenter + Y * nearHeight) - position;
	aux = glm::normalize(aux);
	normal = glm::cross(aux, X);
	pl[TOP].setNormalAndPoint(normal, nearCenter + Y * nearHeight);

	aux = (nearCenter - Y * nearHeight) - position;
	aux = glm::normalize(aux);
	normal = glm::cross(X, aux);
	pl[BOTTOM].setNormalAndPoint(normal, nearCenter - Y * nearHeight);

	aux = (nearCenter - X * nearWidth) - position;
	aux = glm::normalize(aux);
	normal = glm::cross(aux, Y);
	pl[LEFT].setNormalAndPoint(normal, nearCenter - X * nearWidth);

	aux = (nearCenter + X * nearWidth) - position;
	aux = glm::normalize(aux);
	normal = glm::cross(Y, aux);
	pl[RIGHT].setNormalAndPoint(normal, nearCenter + X * nearWidth);
}

int Frustrum::pointInFrustrum(glm::vec3 &p)
{
	int result = INSIDE;
	for (int i = 0; i < 6; i++)
	{
		if (pl[i].distanceFromPlane(p) < 0)
		{
			return OUTSIDE;

		}
	}
	return result;
}

int Frustrum::sphereInFrustrum(glm::vec3 &p, float radius, int& cachedPlane)
{
	float distance;
	int result = INSIDE;

	//if we have a cached plane, check that one first
	if (cachedPlane != -1)
	{
		distance = pl[cachedPlane].distanceFromPlane(p);
		if (distance < -radius)
		{
			return OUTSIDE;
		}
		else if (distance < radius)
		{
			result = INTERSECT;
		}
	}
	for (int i = 0; i < 6; i++)
	{
		//if we already checked a cached plane, don't bother to check it here
		if (i == cachedPlane)
		{
			continue;
		}

		distance = pl[i].distanceFromPlane(p);
		if (distance < -radius)
		{
			cachedPlane = i;
			return OUTSIDE;
		}
		else if (distance < radius)
		{
			result = INTERSECT;
		}
	}
	return result;
}