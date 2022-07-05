#version 450

precision highp float;

in Fragment
{
	vec2 texCoords;
} fragment;

uniform sampler2D tex;

out vec4 finalColor;

void main(void)
{
	finalColor = texture(tex, fragment.texCoords);
}