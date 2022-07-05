#version 330

precision highp float;

uniform mat4 orthoMatrix;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;

uniform vec2 pos;
uniform vec2 size;

out Fragment
{
	vec2 tex_coord;
} fragment;

void main(void)
{
	vec4 outPos = vec4(pos.x + (position.x * size.x), pos.y + (position.y * size.y), 1.0, 1.0);
	gl_Position = orthoMatrix * outPos;
	fragment.tex_coord = texCoords;
}