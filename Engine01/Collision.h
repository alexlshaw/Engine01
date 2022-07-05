#ifndef ENGINE01_COLLISION_H
#define ENGINE01_COLLISION_H

#include "glm\glm.hpp"

const static int X_AXIS = 1;
const static int Y_AXIS = 2;
const static int Z_AXIS = 3;

//Contains code for checking collisions and intersections
namespace Collision
{
	inline bool getIntersection(float fDst1, float fDst2, glm::vec3 p1, glm::vec3 p2, glm::vec3 &hit)
	{
		if (fDst1 * fDst2 >= 0.0f) return false;
		if (fDst1 == fDst2) return false;
		hit = p1 + (p2 - p1) * (-fDst1 / (fDst2 - fDst1));
		return true;
	}

	//checks if the location specified by hit falls within the boundary given by b1-b2 on the specified axis
	inline bool inBox(glm::vec3 hit, glm::vec3 b1, glm::vec3 b2, const int axis)
	{
		if (axis == X_AXIS && hit.z > b1.z && hit.z < b2.z && hit.y > b1.y && hit.y < b2.y) return true;
		if (axis == Y_AXIS && hit.z > b1.z && hit.z < b2.z && hit.x > b1.x && hit.x < b2.x) return true;
		if (axis == Z_AXIS && hit.x > b1.x && hit.x < b2.x && hit.y > b1.y && hit.y < b2.y) return true;
		return false;
	}

	//returns true if an intersection between the line l1-l2 occured in the axis-aligned box b1-b2
	//returns intersection point in hit (closest to l1 if multiple
	//b1 is the minimal corner of the box, b2 is the maximal corner (min/max values on every axis)
	bool checkLineBox(glm::vec3 b1, glm::vec3 b2, glm::vec3 l1, glm::vec3 l2, glm::vec3 &hit);

	//returns true if an intersection between the line l1-l2 occured in the axis-aligned box b1-b2
	//returns an intersection point in hit (not necessarily closest to l1
	//b1 is the minimal corner of the box, b2 is the maximal corner (min/max values on every axis)
	//This method is not ideal if getting the correct hit location is important
	bool checkLineBoxQuick(glm::vec3 b1, glm::vec3 b2, glm::vec3 l1, glm::vec3 l2, glm::vec3 &hit);

	//may not be working correctly (based on old borken version of raySphereCollision)
	bool lineSphereCollision(glm::vec3 lineStart, glm::vec3 lineEnd, glm::vec3 sphereCenter, float sphereRadius, glm::vec3 &hit, glm::vec3 &hitNormal);

	bool raySphereCollision(glm::vec3 rayStart, glm::vec3 rayDir, glm::vec3 sphereCenter, float sphereRadius, glm::vec3 &hit, glm::vec3 &hitNormal);
}

#endif