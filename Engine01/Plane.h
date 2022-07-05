#ifndef ENGINE01_PLANE_H
#define ENGINE01_PLANE_H

#include "glm\glm.hpp"

class Plane
{
private:
public:
	//plane defined as Ax + By + Cz + D = 0
	void setCoefficientsFromPoints(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
	void setNormalAndPoint(glm::vec3 normal, glm::vec3 point);
	glm::vec3 projectPointOnToPlane(glm::vec3 point);
	float distanceFromPlane(glm::vec3 point);
	float A;
	float B;
	float C;
	float D;	//distance from plane to origin
	glm::vec3 normal;
	glm::vec3 planePoint;
};

#endif