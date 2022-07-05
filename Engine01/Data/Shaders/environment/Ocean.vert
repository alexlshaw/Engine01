#version 450

precision highp float;

layout (location = 0) in vec4 position;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out Fragment
{
	vec2 texCoords;
} fragment;

void main(void)
{
	vec4 newPos = modelMatrix * position;
	fragment.texCoords = newPos.xz / 4;
	gl_Position = projectionMatrix * viewMatrix * newPos;
}