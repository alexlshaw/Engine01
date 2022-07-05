#version 450

precision highp float;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform vec3 sunPosition;


out Fragment
{
	vec2 texCoords;
} fragment;

void main(void)
{
	fragment.texCoords = texCoords;
	vec3 cameraRight_world = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	vec3 cameraUp_world = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

	vec3 pos = sunPosition + (cameraRight_world * position.x) + (cameraUp_world * position.z);
	gl_Position = projectionMatrix * viewMatrix * vec4(pos, 1);
}