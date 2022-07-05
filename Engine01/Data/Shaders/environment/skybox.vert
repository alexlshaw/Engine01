#version 450

precision highp float;

layout (location = 0) in vec4 position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 horizon;
uniform vec3 top;

out Fragment
{
	vec3 world_pos;
} fragment;

void main(void)
{
	fragment.world_pos = position.xyz;
	gl_Position = projectionMatrix * viewMatrix * position;
}