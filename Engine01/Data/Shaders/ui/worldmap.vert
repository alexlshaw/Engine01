#version 450

precision highp float;

uniform mat4 projectionMatrix;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;

out Fragment
{
	vec2 tex_coord;
} fragment;

void main(void)
{
	gl_Position = projectionMatrix * vec4(position.xy, 1.0, 1.0);
	fragment.tex_coord = texCoords;
}