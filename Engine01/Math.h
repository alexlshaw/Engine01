#ifndef ENGINE01_MATH_H
#define ENGINE01_MATH_H

#include "glm\glm.hpp"

#define DEGREES_TO_RADIANS      .017453292F
#define RADIANS_TO_DEGREES      57.29577951F
#define PI                      ((double)3.1415926535F)

inline bool within(float n1, float n2, float range)
{
	if (n1 - n2 > range || n2 - n1 > range)
	{
		return false;
	}
	return true;
}

inline float lockToRange(float val, float min, float max)
{
	return val < min ? min : val > max ? max : val;
}

//This is an efficient lerp implementation, but can suffer from precision loss when a and b are far apart
inline float fastLerp(float a, float b, float f)
{
	return a + f * (b - a);
}

bool pointInTriangle(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);

#endif