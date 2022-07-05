#include "Math.h"

bool pointInTriangle(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2)
{
	float A = 0.5f * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
	float sign = A < 0.0f ? -1.0f : 1.0f;
	float s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y) * sign;
	float t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y) * sign;
	return s > 0 && t > 0 && (s + t) < 2 * A * sign;
}