#include "Collision.h"

bool Collision::checkLineBox(glm::vec3 b1, glm::vec3 b2, glm::vec3 l1, glm::vec3 l2, glm::vec3 &hit)
{
	if (l2.x < b1.x && l1.x < b1.x) return false;	//completely out on x axis
	if (l2.x > b2.x && l1.x > b2.x) return false;	//completely out on x axis
	if (l2.y < b1.y && l1.y < b1.y) return false;	//completely out on y axis
	if (l2.y > b2.y && l1.y > b2.y) return false;	//completely out on y axis
	if (l2.z < b1.z && l1.z < b1.z) return false;	//completely out on z axis
	if (l2.z > b2.z && l1.z > b2.z) return false;	//completely out on z axis
	//if the line starts inside the box, the intersection point is the start of our line
	if (l1.x > b1.x && l1.x < b2.x && l1.y > b1.y && l1.y < b2.y && l1.z > b1.z && l1.z < b2.z)
	{
		hit = l1;
		return true;
	}

	//check every side, and find the closest collision to l1
	glm::vec3 closest = l2;
	bool foundHit = false;
	if (getIntersection(l1.x - b1.x, l2.x - b1.x, l1, l2, hit) && inBox(hit, b1, b2, X_AXIS)) 
	{
		closest = hit;
		foundHit = true;
	}
	if (getIntersection(l1.y - b1.y, l2.y - b1.y, l1, l2, hit) && inBox(hit, b1, b2, Y_AXIS))
	{
		if (glm::length(l1 - hit) < glm::length(l1 - closest))
		{
			closest = hit;
		}
		foundHit = true;
	}
	if (getIntersection(l1.z - b1.z, l2.z - b1.z, l1, l2, hit) && inBox(hit, b1, b2, Z_AXIS))
	{
		if (glm::length(l1 - hit) < glm::length(l1 - closest))
		{
			closest = hit;
		}
		foundHit = true;
	}
	if (getIntersection(l1.x - b2.x, l2.x - b2.x, l1, l2, hit) && inBox(hit, b1, b2, X_AXIS))
	{
		if (glm::length(l1 - hit) < glm::length(l1 - closest))
		{
			closest = hit;
		}
		foundHit = true;
	}
	if (getIntersection(l1.y - b2.y, l2.y - b2.y, l1, l2, hit) && inBox(hit, b1, b2, Y_AXIS))
	{
		if (glm::length(l1 - hit) < glm::length(l1 - closest))
		{
			closest = hit;
		}
		foundHit = true;
	}
	if (getIntersection(l1.z - b2.z, l2.z - b2.z, l1, l2, hit) && inBox(hit, b1, b2, Z_AXIS))
	{
		if (glm::length(l1 - hit) < glm::length(l1 - closest))
		{
			closest = hit;
		}
		foundHit = true;
	}
	//make hit store our closest collision point - if we found one
	if (foundHit)
	{
		hit = closest;
	}
	return foundHit;
}

bool Collision::checkLineBoxQuick(glm::vec3 b1, glm::vec3 b2, glm::vec3 l1, glm::vec3 l2, glm::vec3 &hit)
{
	if (l2.x < b1.x && l1.x < b1.x) return false;	//completely out on x axis
	if (l2.x > b2.x && l1.x > b2.x) return false;	//completely out on x axis
	if (l2.y < b1.y && l1.y < b1.y) return false;	//completely out on y axis
	if (l2.y > b2.y && l1.y > b2.y) return false;	//completely out on y axis
	if (l2.z < b1.z && l1.z < b1.z) return false;	//completely out on z axis
	if (l2.z > b2.z && l1.z > b2.z) return false;	//completely out on z axis
	//if the line starts inside the box, the intersection point is the start of our line
	if (l1.x > b1.x && l1.x < b2.x && l1.y > b1.y && l1.y < b2.y && l1.z > b1.z && l1.z < b2.z)
	{
		hit = l1;
		return true;
	}
	//test the line against each face
	if ((getIntersection(l1.x - b1.x, l2.x - b1.x, l1, l2, hit) && inBox(hit, b1, b2, X_AXIS))
		|| (getIntersection(l1.y - b1.y, l2.y - b1.y, l1, l2, hit) && inBox(hit, b1, b2, Y_AXIS))
		|| (getIntersection(l1.z - b1.z, l2.z - b1.z, l1, l2, hit) && inBox(hit, b1, b2, Z_AXIS))
		|| (getIntersection(l1.x - b2.x, l2.x - b2.x, l1, l2, hit) && inBox(hit, b1, b2, X_AXIS))
		|| (getIntersection(l1.y - b2.y, l2.y - b2.y, l1, l2, hit) && inBox(hit, b1, b2, Y_AXIS))
		|| (getIntersection(l1.z - b2.z, l2.z - b2.z, l1, l2, hit) && inBox(hit, b1, b2, Z_AXIS)))
	{
		return true;
	}
	return false;
}

bool Collision::lineSphereCollision(glm::vec3 lineStart, glm::vec3 lineEnd, glm::vec3 sphereCenter, float sphereRadius, glm::vec3 &hit, glm::vec3 &hitNormal)
{
	glm::vec3 lineDir = glm::normalize(lineStart - lineEnd);
	float lineLength = glm::length(lineStart - lineEnd);
	if (raySphereCollision(lineStart, lineDir, sphereCenter, sphereRadius, hit, hitNormal))
	{
		if (glm::length(lineStart - hit) < lineLength)
		{
			return true;
		}
	}
	return false;
	//return raySphereCollision(lineStart, lineDir, sphereCenter, sphereRadius, hit, hitNormal) && glm::length(lineStart - hit) < lineLength;
}

bool Collision::raySphereCollision(glm::vec3 rayStart, glm::vec3 rayDir, glm::vec3 sphereCenter, float sphereRadius, glm::vec3 &hit, glm::vec3 &hitNormal)
{
	glm::vec3 m = rayStart - sphereCenter;
	float b = glm::dot(m, rayDir);
	float c = glm::dot(m, m) - sphereRadius * sphereRadius;
	if (c > 0.0f && b > 0.0f)
	{
		return false;
	}

	float discr = b*b - c;
	if (discr < 0.0f)
	{
		return false;
	}

	float t = -b - glm::sqrt(discr);
	if (t < 0.0f)
	{
		t = 0.0f;
	}

	hit = rayStart + t * rayDir;
	hitNormal = (hit - sphereCenter) / sphereRadius;
	return true;
}