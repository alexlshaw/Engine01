#version 450

precision highp float;

in Fragment
{
	vec2 texCoords;
} fragment;

out vec4 finalColor;

//uniform sampler2D waterTex

void main(void)
{
	//vec3 texel = texture(rockTex, fragment.texCoords).xyz;
	finalColor = vec4(0.5, 0.5, 0.8, 0.4);
}