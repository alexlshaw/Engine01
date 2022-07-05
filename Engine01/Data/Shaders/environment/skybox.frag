#version 450

precision highp float;

const float PI = 3.1415926535897932384626433832795;

uniform vec3 normalizedSunPosition;
uniform vec3 horizon;
uniform vec3 top;

in Fragment
{
	vec3 world_pos;
} fragment;

out vec4 finalColor;

void main(void)
{
	vec3 pointOnSphere = normalize(fragment.world_pos);
	float angleToSun = acos(dot(normalizedSunPosition, pointOnSphere)) / PI;	//gives a value of 0 beside sun, and 1 opposite sun
	float sunFactor = 2.0 - (angleToSun * angleToSun * 1.5);
	vec3 baseColor = horizon + pointOnSphere.y * (top - horizon);
	finalColor = vec4(baseColor * sunFactor, 1);
}