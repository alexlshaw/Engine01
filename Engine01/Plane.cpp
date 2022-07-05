#include "Plane.h"

void Plane::setCoefficientsFromPoints(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
	planePoint = p0;
	glm::vec3 v = p1 - p0;
	glm::vec3 u = p2 - p0;
	normal = glm::normalize(glm::cross(v, u));
	A = normal.x;
	B = normal.y;
	C = normal.z;
	D = glm::dot(-normal, p0);
}

void Plane::setNormalAndPoint(glm::vec3 normal, glm::vec3 point)
{
	planePoint = point;
	this->normal = normal;
	A = normal.x;
	B = normal.y;
	C = normal.z;
	D = glm::dot(-normal, planePoint);
}

glm::vec3 Plane::projectPointOnToPlane(glm::vec3 point)
{
	return point - distanceFromPlane(point) * normal;
}

//distance from a point to the plane. If positive, point is on the same side of the plane as the normal
float Plane::distanceFromPlane(glm::vec3 point)
{
	return A * point.x + B * point.y + C * point.z + D;
}