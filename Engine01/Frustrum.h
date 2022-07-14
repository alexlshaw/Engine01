#ifndef ENGINE01_FRUSTRUM_H
#define ENGINE01_FRUSTRUM_H

#include "Math.h"
#include "Plane.h"
#include "Settings.h"

class Frustrum
{
private:
	enum {
		TOP = 0, BOTTOM = 1, LEFT = 2, RIGHT = 3, NEARP = 4, FARP = 5
	};
public:
	Plane pl[6];
	float nearDistance, farDistance, ratio, angle, tang;
	float nearWidth, nearHeight, farWidth, farHeight;	//near/far width/height
	Frustrum();
	~Frustrum();
	void setCamInternals(float angle, float ratio, float nearDistance, float farDistance);	//called once when initialising (or when camera basic properties change)
	void setCamDef(glm::vec3 position, glm::vec3 look, glm::vec3 up);
	int pointInFrustrum(glm::vec3 &p);
	int sphereInFrustrum(glm::vec3 &p, float radius, int& cachedPlane);
};

#endif